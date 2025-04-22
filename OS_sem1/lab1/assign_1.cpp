#include <iostream>
#include <string.h>
#include <string>
#include <vector> 
#include <unordered_map>
#include <map>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include <regex>
using namespace std;

// ++++++++++++++++++ All global variables ++++++++++++++++++
// FOR SYMBOL TABLE
// 1. vector stores the key values in order of insertion
// 2. unordered map stored the key value pairs
vector<string> symbols;
unordered_map<string, int> symbol_table;

// FOR MEMORY TABLE
// 1. Use a ordered map as modules are numbere from 0 to n
// 2. key is module number and value is the base address calculated
vector<int> module_list;
map<int, int> memory_base_table;


// FOR where is the parser at used during parse 1 and parse 2
struct parser_metadata{
    bool at_deflist;
    bool read_defcount;
    bool read_S;
    bool read_R;
    bool at_useliset;
    bool read_usecount;
    bool read_symbols;
    bool at_program_text;
    bool read_codecount;
    bool read_mode;
    bool read_instruction;
    bool dup_updated;
};  
parser_metadata parser_meta;
// FOR delim used in strtok
// using const as delim is independent of any input
const char* delim = " \t\n";

// Each module has defcount, usecount, codecount
// which then tells how many arguments need to be parsed
int defcount, usecount, codecount;

//for module
vector<string> module_symbols; //get flushed after every module
vector<int> module_symbol_rel_addr;

//for parse 1
int linenum = 0;
int lineoffset = 0;
vector<string> duplicates;
//for parse 2
string mode;
int memory_num = 0;
vector<string> uselist_symbols; //get flushed after every module
map<int, vector<string> > all_symbols; // storing which module has which symbol for warning ins parse 2
unordered_map<string, bool> ifused; //to check if a symbol is used, to print warning after parse 2
unordered_map<string, bool> is_referenced; // to check for warning at each module if a symbol is referenced
unordered_map<string, bool> capture_reference;
// +++++++++++++++++++++ ALL function definitons +++++++++++++++++++++++

void reset_module();
char* get_token();
void validate_token(string token_type, char* val);
char* change_string_type(string line);

void parser_1(int iteration, ifstream &input_file_handler);
void parser_2(int iteration, ifstream &input_file_handler);
int whereisparser_at();
int whereistoken(char* val, string line);
void initialise_parser_meta();
void parse_error(int num);
void was_i_expecting_anything();

void do_at_deflist(char* val);
int read_defcount(char* defcount_val);
void do_at_uselist(char* val);
void do_at_program_text(char* val);

void update_symbol_table(char* symbol_or_reladdr);
void update_memory_base_table();



void do_at_deflist(char* val);
void do_when_reading_defcount(char* val);
void do_when_reading_s_at_deflist(char* val);
void do_when_reading_r_at_deflist(char* val);

void do_at_uselist(char* val);
void do_when_reading_usecount(char* val);
void do_when_reading_symbols_at_uselist(char* val);

void do_at_program_text(char* val);
void do_when_reading_codecount(char* val);
void do_when_reading_mode_at_codelist(char* val);
void do_when_reading_instr_at_codelist(char* val);

void do_at_deflist2(char* val);
void do_when_reading_defcount2(char* val);
void do_when_reading_s_at_deflist2(char* val);
void do_when_reading_r_at_deflist2(char* val);

void do_at_uselist2(char* val);
void do_when_reading_symbols_at_uselist2(char* val);

void do_at_program_text_2(char* val);
void do_when_reading_mode_at_codelist2(char* val);
void do_when_reading_instr_at_codelist2(char* val);

int read_defcount(char* defcount_val);
int read_usecount(char* val);
int read_codecount(char* val);

void update_symbol_table(string symbol);
void update_module_table(int codecount);

void do_operation_M(char* val);
void do_operation_A(char* val);
void do_operation_R(char* val);
void do_operation_I(char* val);
void do_operation_E(char* val);
bool is_opcode_valid(int opcode);

void print_symbol_table();
void print_error(int num);
void print_error(int num, string symbol);
void print_warning(int module);
string to_upper_case(string mode);

void print_warning_invalid_rel_addr(int module, string symbol, int rel_addr, int valid_addr);
void print_warning_ignore_redef(int module, string symbol);
void print_warning_symbol_not_referenced(int module, string symbol, int index);
void print_warning_symbol_not_used(int module, string symbol);

void check_unused_symbols(); //for printing warning rule 4
void check_if_unreferenced_symbols(); //for printing warning rule 7
int main(int argc, char* argv[])
{
    
    //open file and send filehandler to tokenizer
    string file_name = change_string_type(argv[0]);
    ifstream input_file(argv[1]);
    // First parse
    if(input_file.is_open())
    {
        reset_module();
        initialise_parser_meta();
        // printf("starting parse 1");
        parser_1(1,input_file);
        input_file.close();
    }

    // Print symbol table
    print_symbol_table();
    printf("\nMemory Map");

    //  Second Parse
    input_file.open(argv[1]);
    if(input_file.is_open())
    {
        module_list.clear();
        reset_module();
        initialise_parser_meta();
        // printf("starting parse 2");
        parser_2(2, input_file);
        input_file.close();
    }
}

void parser_1(int iteration, ifstream &input_file_handler)
{
    char* token;
    bool first = true;
    
    while(input_file_handler && !input_file_handler.eof())
    {
        string line;
        getline(input_file_handler, line);
        linenum ++;
        lineoffset = 1;
        int size_;
        char* line_ = change_string_type(line);
        char* token;
        const char* delim = " \t\n";
        token = strtok(line_, delim); //first token of any line in file
        while(token != nullptr)
        {
            lineoffset = token - line_ + 1;
            switch(whereisparser_at())
            {
                case 1:
                    // printf("\nAT DEFLIST:defcount:%d\t",defcount);
                    do_at_deflist(token);
                    break;
                case 2:
                    // printf("\nAT USELIST:usecount:%d\t",usecount);
                    do_at_uselist(token);
                    break;
                case 3:
                    // printf("\nAT PROGRAMTEXT:codecount:%d\t",codecount);
                    do_at_program_text(token);
                    break;
            }
            // printf("%s\n", token);
            if(token!=nullptr)
            {
                size_ = strlen(token);
            }
            
            token = get_token(); //subsequent tokens

        }
        if(token == nullptr && input_file_handler.eof())
        {
            lineoffset += size_;
            was_i_expecting_anything();
        }
    }
}

void parser_2(int iteration, ifstream &input_file_handler)
{
    char* token;
    bool first = true;
    while(input_file_handler && !input_file_handler.eof())
    {
        string line;
        getline(input_file_handler, line);
        char* line_ = change_string_type(line);
        char* token;
        const char* delim = " \t\n";
        token = strtok(line_, delim); //first token of any line in file
        
        while(token != nullptr)
        {
            
            switch(whereisparser_at())
            {
                case 1:
                    // printf("\nAT DEFLIST:defcount:%d\t",defcount);
                    do_at_deflist2(token);
                    break;
                case 2:
                    // printf("\nAT USELIST:usecount:%d\t",usecount);
                    do_at_uselist2(token);
                    break;
                case 3:
                    // printf("\nAT PROGRAMTEXT:codecount:%d\t",codecount);
                    do_at_program_text_2(token);
                    break;
            }
            // printf("%s\n", token);
            token = get_token(); //subsequent tokens
        }
        
    }
    check_unused_symbols();
}
char* get_token()
{
    return strtok(nullptr, delim);
}

void validate_token(string token_type, char* val)
{
    char* temp = val;
    if(token_type.compare("number")==0)
    {
        //validate integer
        // validate <2^30
        //else parse error --> num expected
        try
        {
            {
                int num = stoi(val);
                if(num >= 1073741824)
                {
                    parse_error(3);
                }
            }
        }
        catch(const std::invalid_argument& e)
        {
            // printf("invalid\t %s",val);
            parse_error(3);
        }
        catch (const std::out_of_range& e) {
            // printf("ooo \t %s",val);
            parse_error(3);
        }

    }
        
    else if(token_type.compare("symbol")==0)
    {
        //valid symbol
        //validate symbols upto 16 chars -> symbol too long
        //else parse error --> sym expected
        string symbol_ = change_string_type(temp);
        // printf("VALIDATING %s token", temp);
        int len = symbol_.size();
        if(len>16)
        {
            parse_error(6);
        }
        regex pattern("[A-Za-z][A-Za-z0-9]*");
        if( ! regex_match(symbol_, pattern))
        {
            parse_error(4); //sym expected
        }
    }

    else if(token_type.compare("marie")==0)
    {
        string uc_mode = change_string_type(temp);
        if(uc_mode.compare("M")==0)
        {
            return;
        }
        else if (uc_mode.compare("A")==0)
        {
            return;
        }
        else if (uc_mode.compare("R")==0)
        {
            return;
        }
        else if (uc_mode.compare("I")==0)
        {
            return;
        }
        else if (uc_mode.compare("E")==0)
        {
            return;
        }
        else{
            parse_error(5);
        }
        //else parse error --> marie expected
    }

    else
    {
        // printf("\ncontrol should never be here, erronous code");
    }
}

void reset_module()
{
    defcount = -1;
    usecount = -1;
    codecount = -1;
}

char* change_string_type(string line)
{
    char* c_str = new char[line.length() + 1];
    strcpy(c_str, line.c_str());
    return c_str;
}

int whereisparser_at()
{
    if(parser_meta.at_deflist)
    {
        return 1;
    }
    else if(parser_meta.at_useliset)
    {
        return 2;
    }
    else if(parser_meta.at_program_text)
        {
            return 3;
        }
    else
    {
        // printf("DEAD");
        exit(1);
    }

}

void initialise_parser_meta()
{
    parser_meta.at_deflist = true;
    parser_meta.read_defcount = true; 
    parser_meta.read_S = false;
    parser_meta.read_R = true;
    parser_meta.at_useliset = false;
    parser_meta.read_symbols = false;
    parser_meta.at_program_text = false;
    parser_meta.read_mode = false;
    parser_meta.read_instruction = false;
    parser_meta.dup_updated = false;
}
void do_at_deflist(char* val){
    if(parser_meta.read_defcount)
    {
        reset_module();
        validate_token("number", val);
        do_when_reading_defcount(val);
        if(defcount > 0)
        {
            parser_meta.read_defcount = false; //defcount has been read
            parser_meta.read_S = true;
        }
        else
        {
            parser_meta.at_deflist = false;
            parser_meta.at_useliset = true;
            parser_meta.read_defcount = false;
            parser_meta.read_usecount = true;
        }
    }

    else if(parser_meta.read_S)
    {
        validate_token("symbol", val);
        do_when_reading_s_at_deflist(val);
        parser_meta.read_S = false;
        parser_meta.read_R = true ;
    }

    else if (parser_meta.read_R)
    {
        if(parser_meta.dup_updated)
        {
            // printf("AM I HERE?");
            parser_meta.dup_updated = false;
            defcount -= 1;
            parser_meta.read_R = false;
            module_symbol_rel_addr.push_back(atoi(val));
            if(defcount==0){
                parser_meta.at_deflist = false;
                parser_meta.at_useliset = true;
                parser_meta.read_usecount = true;
            }
            else{
                parser_meta.read_S = true;
            }
                return;
        }
        validate_token("number", val);
        do_when_reading_r_at_deflist(val);
        parser_meta.read_R = false;
        if(defcount==0){
            parser_meta.at_deflist = false;
            parser_meta.at_useliset = true;
            parser_meta.read_usecount = true;
        }
        else{
            parser_meta.read_S = true;
        }
    }

    else{
        // printf("DEAD");
    }
    

}

void do_when_reading_defcount(char* val)
{
    char* temp = val;
    defcount = read_defcount(temp);
    if(defcount>16)
    {
        parse_error(0); //to many def in module
    }
    if(defcount>-1) //update module_list only if defcount is updated and valid
    {
        // if we have a new defcount, it implies we have a new module
        // here we update the vector module list
        // module table can be updated only with the number of instructions
        // present at program text
        if(module_list.empty())
        {
            // printf("updating empty module list ");
            module_list.push_back(0);
        }
        else{
           
            int prev_module = module_list.back();
            // printf("updating modulelist with new module\t%d", prev_module+1);
            module_list.push_back(prev_module+1);
        }
        
        }
}

void do_when_reading_s_at_deflist(char* val)
{
    string symbol_ = change_string_type(val);
    module_symbols.push_back(symbol_); //stalling the deflist for a module
    //check if it already is defined
    auto duplicate = find(symbols.begin(), symbols.end(), symbol_);
    if (duplicate != symbols.end())
    {
        // printf("\nHere at reading s, as a duplicate\n");
        parser_meta.dup_updated = true;
        // printf("#####DOOOP#####\t %s", val);
        auto duplicate_ = find(duplicates.begin(), duplicates.end(), symbol_);
        if(duplicate_ != duplicates.end())
        {
            // print_warning_ignore_redef(module_list.back(),symbol_);
        }
        else
        {
            // printf("i should be here");
            // print_warning_ignore_redef(module_list.back(),symbol_);
            duplicates.push_back(symbol_);
        }
        
    }
    else
    {
        // printf("\nHere at reading s, NOT as a duplicate\n");
        parser_meta.dup_updated = false;
        symbols.push_back(symbol_);
    }
    
    
}

void do_when_reading_r_at_deflist(char* val)
{
    //validate r
    //update symbol table with rel addr
    string symbol_ = symbols.back();
    module_symbol_rel_addr.push_back(atoi(val));
    auto duplicate = find(duplicates.begin(), duplicates.end(), symbol_);
    if (duplicate != duplicates.end())
    {
        // printf("\nAM I here %s\t",symbol_.c_str());
        // for (size_t i = 0; i < symbols.size(); ++i) {
        // std::cout << symbols[i] << std::endl;
        //  }
        defcount -= 1;//pass; symbol already exists, do not update value
    }
    else
    {
        int curr_module = module_list.back();
        int curr_base_addr = memory_base_table[curr_module];
        symbol_table[symbol_] =  atoi(val) + curr_base_addr;
        // printf("################BEECH main###########");
        // print_symbol_table();
        // printf("#####################################");
        defcount -= 1;
    }
    
    
}

void do_at_uselist(char* val){
    if(parser_meta.read_usecount)
    {
        validate_token("number",val);
        do_when_reading_usecount(val);
        if(usecount > 0 )
        {
            parser_meta.read_symbols = true;
            parser_meta.read_usecount = false;
        }
        else{
            parser_meta.at_useliset = false;
            parser_meta.at_program_text = true;
            parser_meta.read_codecount = true;
            parser_meta.read_symbols = false;
        }
    }
    else if(parser_meta.read_symbols)
    {
        validate_token("symbol", val);
        do_when_reading_symbols_at_uselist(val);
        if(usecount==0)
        {
            parser_meta.at_useliset = false;
            parser_meta.at_program_text = true;
            parser_meta.read_codecount = true;
            parser_meta.read_symbols = false;
        }
    }
}
void do_at_uselist2(char* val)
{
    if(parser_meta.read_usecount)
    {
        do_when_reading_usecount(val);
        if(usecount > 0 )
        {
            parser_meta.read_symbols = true;
            parser_meta.read_usecount = false;
        }
        else{
            parser_meta.at_useliset = false;
            parser_meta.at_program_text = true;
            parser_meta.read_codecount = true;
            parser_meta.read_symbols = false;
        }
    }
    else if(parser_meta.read_symbols)
    {
        
        do_when_reading_symbols_at_uselist2(val);
        if(usecount==0)
        {
            parser_meta.at_useliset = false;
            parser_meta.at_program_text = true;
            parser_meta.read_codecount = true;
            parser_meta.read_symbols = false;
            // printf("\n$$$$$$$USELISTTTTTT");
            // for (size_t i = 0; i < uselist_symbols.size(); ++i) {
            //     std::cout << uselist_symbols[i] << " ";
            // }
        }
    }

}
void do_when_reading_usecount(char* val)
{
    //validate(usecount)
    usecount = read_usecount(val);
    if(usecount>16)
    {
        parse_error(1);
    }
    if(usecount>-1)
    {
        //since usecount is already validated to be an integer
        // now, validate syntax
    }
}

void do_when_reading_symbols_at_uselist(char* val)
{
    //validate symbols
    //check if symbol in symbol table
    //decrement usecount
    usecount -= 1;
}
void do_when_reading_symbols_at_uselist2(char* val)
{
    string symbol_ = change_string_type(val);
    uselist_symbols.push_back(symbol_);
    ifused[symbol_] = true; // for printing pwarning that symbol defined but never used
    usecount -= 1;
}

void do_at_program_text(char* val){
    if(parser_meta.read_codecount)
    {
        validate_token("number", val);
        do_when_reading_codecount(val);
        if(codecount > 0)
        {
            parser_meta.read_codecount = false;
            parser_meta.read_mode = true;
        }
        else
        {
            parser_meta.at_program_text = false;
            parser_meta.read_mode = false;
            parser_meta.read_instruction = false;
            parser_meta.at_deflist = true;
            parser_meta.read_defcount = true;

        }
        
    }
    else if(parser_meta.read_mode)
    {
        validate_token("marie", val);
        do_when_reading_mode_at_codelist(val);
        parser_meta.read_mode = false;
        parser_meta.read_instruction = true;
    }
    else if(parser_meta.read_instruction)
    {
        do_when_reading_instr_at_codelist(val);
        if(codecount == 0)
        {
            parser_meta.at_program_text = false;
            parser_meta.read_mode = false;
            parser_meta.read_instruction = false;
            parser_meta.at_deflist = true;
            parser_meta.read_defcount = true;
        }
        else
        {
            parser_meta.read_mode = true;
            parser_meta.read_instruction = false;
        }
    }


}

void do_when_reading_codecount(char* val)
{
    //validate codecount
    codecount = read_codecount(val);
    //check deflist for warning
    if(codecount>512)
    {
        // parse_error(2);
        print_error(3);
    }

    if(codecount>-1)
    {
        //syntactically ok to read (M,I)
        update_module_table(codecount);
        for (int i = 0; i < module_symbols.size(); ++i) 
        {
            // printf("entering the rel_Addr check ");
            
            string symbol_ = module_symbols[i];
            int rel_addr = module_symbol_rel_addr[i];
            // printf("%d\t%s\t%d\n",i,symbol_.c_str(), rel_addr);
            auto duplicate_ = std::find(duplicates.begin(), duplicates.end(), symbol_);
            if(rel_addr>=codecount)
            {
                if(duplicate_ != duplicates.end())
                {
                    print_warning_ignore_redef(module_list.back(),symbol_);
                }
                else
                {
                    symbol_table[symbol_]=symbol_table[symbol_]-rel_addr;
                    print_warning_invalid_rel_addr(module_list.back(), symbol_, rel_addr, codecount-1);
                }
                
            }
        }
        module_symbols.clear();
        module_symbol_rel_addr.clear();

    }
}

void do_when_reading_mode_at_codelist(char* val)
{
    //validate mode , i.e MARIE
}

void do_when_reading_instr_at_codelist(char* val)
{
    //validate instruction opcode and operand in parse 1
    //make changes based on R in parse 2
    codecount -= 1;
}

void do_at_program_text_2(char* val){
    if(parser_meta.read_codecount)
    {
        do_when_reading_codecount(val);
        if(codecount > 0)
        {
            parser_meta.read_codecount = false;
            parser_meta.read_mode = true;
        }
        else
        {
            parser_meta.at_program_text = false;
            parser_meta.read_mode = false;
            parser_meta.read_instruction = false;
            parser_meta.at_deflist = true;
            parser_meta.read_defcount = true;
            check_if_unreferenced_symbols();
            uselist_symbols.clear();
            is_referenced.clear();

        }
        
    }
    else if(parser_meta.read_mode)
    {
        do_when_reading_mode_at_codelist2(val);
        parser_meta.read_mode = false;
        parser_meta.read_instruction = true;
    }
    else if(parser_meta.read_instruction)
    {
        do_when_reading_instr_at_codelist2(val);
        if(codecount == 0)
        {
            parser_meta.at_program_text = false;
            parser_meta.read_mode = false;
            parser_meta.read_instruction = false;
            parser_meta.at_deflist = true;
            parser_meta.read_defcount = true;
            // because module over, we can flush the uselist stored for 
            // accessing index
            
            check_if_unreferenced_symbols();
            is_referenced.clear();
            uselist_symbols.clear();
        }
        else
        {
            parser_meta.read_mode = true;
            parser_meta.read_instruction = false;
        }
    }


}
void do_when_reading_mode_at_codelist2(char* val)
{
    //storing mode to do operation
    mode = *val;
}

void do_when_reading_instr_at_codelist2(char* val)
{
    mode = to_upper_case(mode);
    char mode_ = mode[0];

    switch(mode_)
    {
        case 'M':
            do_operation_M(val);
            break;
        case 'A' :
            do_operation_A(val);
            break;
        case 'R':
            do_operation_R(val);
            break;
        case 'I':
            do_operation_I(val);
            break;
        case 'E':
            do_operation_E(val);
            break;
    }
    codecount -= 1;
}
void do_operation_M(char* val)
{
    // operand is the number of a valid module and 
    // is replaced with the base address of that module
    
    int instr_ = atoi(val);
    int operand = instr_ % 1000;
    int opcode = instr_ / 1000;
    if(!is_opcode_valid(opcode))
    {
        return;
    }
   
    int max_module = memory_base_table.size()-1;
    if(operand<max_module)
    {
        int base_addr_ = memory_base_table[operand];
        int new_instr = opcode * 1000 + base_addr_;
        printf("\n%03d: %04d",memory_num++,new_instr);
    }
    else
    {
        int new_instr = opcode * 1000 + 0;
        printf("\n%03d: %04d",memory_num++,new_instr);
        print_error(8);
    }
    return;
}
void do_operation_A(char* val)
{
    // operand is an absolute address which will never be changed in pass2; 
    // however it can’t be “>=” the machine size (512);  
    int instr_ = atoi(val);
    int operand = instr_ % 1000;
    int opcode = instr_ / 1000;
    if(!is_opcode_valid(opcode))
    {
        return;
    }
    if(operand >= 512)
    {
        int new_instr_ = opcode * 1000 + 0 ;
        printf("\n%03d: %04d",memory_num++,new_instr_);
        print_error(4);
    }
    else 
    {
        printf("\n%03d: %04d",memory_num++,instr_);
    }
    
}
void do_operation_R(char* val)
{
    // operand is a relative address in the module which is 
    // relocated by replacing the relative address with the 
    // absolute address of that relative address after the module’s global address has been determined 
    // (absolute_addr = module_base+relative_addr)
    int instr_ = atoi(val);
    // printf("\n$$$$$$$MODULELISTTTTTT\t");
    // for (size_t i = 0; i < module_list.size(); ++i) {
    //     std::cout << module_list[i] << " ";
    // }
    int curr_module = module_list.back();
    int base_addr_ = memory_base_table[curr_module];
    int operand = instr_ % 1000;
    int opcode = instr_ / 1000;
    if(!is_opcode_valid(opcode))
    {
        return;
    }
    int abs_addr_ = operand + base_addr_;
    int new_instr = opcode*1000 + abs_addr_;
    int next_base_addr = memory_base_table[curr_module+1];
    if(abs_addr_ > next_base_addr)
    {
        int new_instr = opcode*1000 + base_addr_;
        printf("\n%03d: %04d",memory_num++,new_instr);
        print_error(5);
        return;
    }
    printf("\n%03d: %04d",memory_num++,new_instr);

    
}
void do_operation_I(char* val)
{
    // an immediate operand is unchanged, but must be less than 900.
    int instr = atoi(val);
    int opcode = instr/1000;
    if(!is_opcode_valid(opcode))
    {
        return;
    }
    int operand = instr % 1000;
    if(operand >= 900)
    {
        int new_instr = opcode*1000 + 999;
        printf("\n%03d: %04d",memory_num++,new_instr);
        print_error(6);
        return;
    }
    printf("\n%03d: %04d",memory_num++,instr);
}
void do_operation_E(char* val)
{
    // operand is an external address which is represented as an index into the uselist. 
    // For example, a reference in the program text with operand K represents the Kth symbol in the use list, 
    // using 0-based counting, e.g., if the use list is ‘‘2 f g’’, 
    // then an instruction ‘‘E 7000’’ refers to f, and an instruction ‘‘E 5001’’ refers to g. 
    // You must identify to which global address the symbol is assigned and then replace the operand with that global address.
    int instr_ = atoi(val);
    int operand = instr_ % 1000;
    int opcode = instr_ / 1000;
    // printf("#################printing symmbols#########");
    // for(int i =0; i < uselist_symbols.size();++i)
    // {
    //     printf("%s\t",uselist_symbols[i].c_str());
    // }
    if(!is_opcode_valid(opcode))
    {
        return;
    }
    // printf("size of uselist %d and operand %d \n",uselist_symbols.size(), operand);
    int comp = uselist_symbols.size() -1;
    if(comp < operand)
    {
        int new_instr_ = opcode * 1000 + memory_base_table[module_list.back()];
        printf("\n%03d: %04d",memory_num++,new_instr_);
        print_error(3);
    }
    
    else 
    {
        // printf("Entering Else\n");
        // printf("size of uselist %d and operand %d \n",uselist_symbols.size(), operand);
        string symbol_ = uselist_symbols.at(operand);
        is_referenced[symbol_] = true;
        capture_reference[symbol_] = true;
        auto temp_symbol = find(symbols.begin(), symbols.end(), symbol_);
        if(temp_symbol == symbols.end()) // symbol not defined anywhere
        {
            int new_instr_ = opcode * 1000 + 0 ;
            printf("\n%03d: %04d",memory_num++,new_instr_);
            print_error(1, symbol_);
        }
        else{
            int abs_addr_ = symbol_table[symbol_];
            int new_instr_ = opcode * 1000 + abs_addr_ ;
            printf("\n%03d: %04d",memory_num++,new_instr_);
        }   
    }
}
bool is_opcode_valid(int opcode)
{
    bool status;
    opcode>9 ? status = false:  status = true;
    if(!status)
    {
        printf("\n%03d: 9999",memory_num++);
        print_error(7);
    }
    return status;
}
int read_defcount(char* defcount_val)
{
    // printf("\ndefcount: %d\n", atoi(defcount_val));
    return atoi(defcount_val);
}

int read_usecount(char* val)
{
    // printf("\nusecount: %d\n", atoi(val));
    return atoi(val);
}
int read_codecount(char* val)
{
    // printf("\ncodecount: %d\n", atoi(val));
    return atoi(val);
}
void update_symbol_table(string symbol)
{
    // if(parser_meta.read_S)
    // {
    //     symbols.push_back(symbol);
    // }
    // else if(parser_meta.read_R)
    // {
    //     symbol = symbols.back();
    //     int curr_module = module_list.back();
    //     symbol_table[symbol]= memory_base_table[curr_module] + r_value ;
    // }


}

void update_module_table(int codecount)
{
    memory_base_table[0] = 0;
    int module_num = module_list.back();
    int curr_base_addr = memory_base_table[module_num];
    int next_base_address = codecount + curr_base_addr;
    if(next_base_address>512)
    {
        parse_error(2);
    }
    memory_base_table[module_num+1] = next_base_address;

}

void print_symbol_table()
{
    // printf("\n$$$$$$$DOOOPES");
    // for (size_t i = 0; i < duplicates.size(); ++i) {
    //     std::cout << duplicates[i] << " ";
    // }
    // printf("\n$$$$$$$MODULELISTTTTTT\t");
    // for (size_t i = 0; i < module_list.size(); ++i) {
    //     std::cout << module_list[i] << " ";
    // }
    // for (int i = 0; i<module_list.size(); ++i)
    // {
    //     printf("\n%d=%d\n",module_list[i],memory_base_table[module_list[i]]);
    // }
    printf("Symbol Table");
    for (auto it = symbols.begin(); it != symbols.end(); ++it) {
        string symbol_ = *it ;
        auto duplicate = find(duplicates.begin(), duplicates.end(), symbol_);
        int addr_ = symbol_table[symbol_];
        printf("\n%s=%d", symbol_.c_str(),addr_);
        if (duplicate != duplicates.end())
        {
            print_error(1);
        }
    }
}

string to_upper_case(string mode)
{
    for (char& c : mode) {
        c = std::toupper(static_cast<unsigned char>(c)); 
    }
    return mode;
}

void print_error(int num)
{
    switch(num){
        case 1: 
            printf(" Error: This variable is multiple times defined; first value used");
            break;
        case 2:
            printf("Error: %s is not defined; zero used", mode.c_str());
            break;
        case 3:
            printf(" Error: External operand exceeds length of uselist; treated as relative=0");
            break;
        case 4:
            printf(" Error: Absolute address exceeds machine size; zero used");
            break;
        case 5:
            printf(" Error: Relative address exceeds module size; relative zero used");
            break;
        case 6:
            printf(" Error: Illegal immediate operand; treated as 999");
            break;
        case 7:
            printf(" Error: Illegal opcode; treated as 9999");
            break;
        case 8:
            printf(" Error: Illegal module operand ; treated as module=0");
            break;
    }   
}
void print_error(int num, string symbol)
{
    switch(num)
    {
        case 1:
            printf(" Error: %s is not defined; zero used", symbol.c_str());
            break;

    }
}

void print_warning_invalid_rel_addr(int module, string symbol, int rel_addr, int valid_addr)
{
    printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", module, symbol.c_str(), rel_addr, valid_addr);
}
void print_warning_ignore_redef(int module, string symbol)
{
    printf("Warning: Module %d: %s redefinition ignored\n", module, symbol.c_str());
}
        
void print_warning_symbol_not_referenced(int module, string symbol, int index)
{
    printf("\nWarning: Module %d: uselist[%d]=%s was not used", module, index, symbol.c_str());
}
        
void print_warning_symbol_not_used(int module, string symbol)
{
    printf("\nWarning: Module %d: %s was defined but never used",module,symbol.c_str());
}




void do_at_deflist2(char* val){
    if(parser_meta.read_defcount)
    {
        reset_module();
        do_when_reading_defcount2(val);
        if(defcount > 0)
        {
            parser_meta.read_defcount = false; //defcount has been read
            parser_meta.read_S = true;
        }
        else
        {
            parser_meta.at_deflist = false;
            parser_meta.at_useliset = true;
            parser_meta.read_usecount = true;
        }
    }

    else if(parser_meta.read_S)
    {
        do_when_reading_s_at_deflist2(val);
        parser_meta.read_S = false;
        parser_meta.read_R = true ;
    }

    else if (parser_meta.read_R)
    {

        do_when_reading_r_at_deflist2(val);
        parser_meta.read_R = false;
        if(defcount==0){
            parser_meta.at_deflist = false;
            parser_meta.at_useliset = true;
            parser_meta.read_usecount = true;
        }
        else{
            parser_meta.read_S = true;
        }
    }

    else{
        // printf("DEAD");
    }
    

}

void do_when_reading_defcount2(char* val)
{
    char* temp = val;
    //validate(defcount)
    defcount = read_defcount(temp);
    if(defcount>-1) //update module_list only if defcount is updated and valid
    {
        // if we have a new defcount, it implies we have a new module
        // here we update the vector module list
        // module table can be updated only with the number of instructions
        // present at program text
        if(module_list.empty())
        {
            // printf("updating empty module list ");
            module_list.push_back(0);
        }
        else{
           
            int prev_module = module_list.back();
            // printf("updating modulelist with new module\t%d", prev_module+1);
            module_list.push_back(prev_module+1);
        }
        if (defcount==0)
            {
                all_symbols[module_list.back()].push_back("000") ;
            }
        
        }
}

void do_when_reading_s_at_deflist2(char* val)
{
    // printf("I am entering the function");
    string val_ = change_string_type(val);
    int curr_module = module_list.back();
    for(int i=0; i<=curr_module; i++)
    {
        auto temp = find(all_symbols[i].begin(), all_symbols[i].end(), val_);
        if(temp!=all_symbols[i].end())
        {
            return; 
            // is a duplicate, need not do anything
        }
    }
    // printf("\n\n###################UPDATING ALLSYMBOL###################\n");
    all_symbols[curr_module].push_back(val_);
}

void do_when_reading_r_at_deflist2(char* val)
{
    defcount -= 1;
    return;
}

void check_unused_symbols()
{
    int last_module = module_list.back();
    // for (const auto& pair : ifused) {
    //     std::cout << "\nKey: " << pair.first << ", Value: " << pair.second << std::endl;
    // }
    
    // printf("\nsIZEE%lu",all_symbols[1].size());
    // printf("\n%d",last_module);
    // printf("\n%s",all_symbols[1].back().c_str());
    for (int i =0; i<=last_module; i++)
    {
        // printf("\ni\t%d",i);
        vector<string> curr_symbols = all_symbols[i];
        // printf("\ncurr_size\t%lu",curr_symbols.size());
        for(int j=0; j<curr_symbols.size(); j++)
        {
            // printf("\nj\t%d\t%s",j,curr_symbols[j].c_str());
            string temp = curr_symbols[j];
            // printf("\ntemp %s", temp.c_str());
            try
            {
                bool no_error = ifused.at(temp);
                bool referenced_ = capture_reference.at(temp);
            }
            catch(const std::out_of_range& e)
            {
                // printf("\nin catch blockkk\t%s\n",temp.c_str());
                if(temp.compare("000")==0)
                {
                    continue;
                }
                // printf("\nendin catch blockkk\t%s\n",temp.c_str());
                print_warning_symbol_not_used(i,temp);
            }
            
            
        }

    }
}

void check_if_unreferenced_symbols()
{
    // printf("\n @@@@@@@@@@@@@checking unrefenced symbols@@@@@@@@@@@");
    int module_num = module_list.back();
    for(int i =0; i < uselist_symbols.size();++i)
    {
        string temp = uselist_symbols[i];
        try
        {
            bool no_error = is_referenced.at(temp);
            if(no_error)
            {
                // printf("AM I HERE %d\n", module_num);
                continue;
            }
        }
        catch(const std::out_of_range& e )
        {
            print_warning_symbol_not_referenced(module_num, temp, i);
        }
        
    }
    // uselist_symbols.clear();
}
void was_i_expecting_anything()
{
    
    if(parser_meta.read_defcount && parser_meta.at_deflist)
    {
        return;
    }
    else if(parser_meta.at_deflist && (parser_meta.read_R || parser_meta.read_S))
    {
        if(parser_meta.read_S)
        {
            parse_error(4);
        }
        else
        {
            parse_error(3);
        }
    }
    else if(parser_meta.at_useliset && parser_meta.read_symbols)
    {
        parse_error(4);
    }
    else if(parser_meta.at_program_text && (parser_meta.read_mode || parser_meta.read_instruction))
    {
        if(parser_meta.read_mode)
        {
            parse_error(5);
        }
        else
        {
            parse_error(3);
        }
    }
}
void parse_error(int num)
{
    static string errstr[] = {
        "TOO_MANY_DEF_IN_MODULE", //>16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
        "NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either // Symbol Expected
        "SYM_EXPECTED", // Symbol Expected
        "MARIE_EXPECTED",// Addressing Expected which is M/A/R/I/E
        "SYM_TOO_LONG",// Symbol Name is too long
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[num].c_str()); 
    exit(1);
}

