#include <iostream>
#include <string.h>
#include <string>
#include <vector> 
#include <unordered_map>
#include <map>
#include <fstream>
#include <cstdio>
#include <stdio.h>
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
};  
parser_metadata parser_meta;
// FOR delim used in strtok
// using const as delim is independent of any input
const char* delim = " \t\n";

// Each module has defcount, usecount, codecount
// which then tells how many arguments need to be parsed
int defcount, usecount, codecount;



// +++++++++++++++++++++ ALL function definitons +++++++++++++++++++++++

void reset_module();
char* get_token();
bool validate_token(string token_type);
char* change_string_type(string line);

void parser(int iteration, ifstream &input_file_handler);
int whereisparser_at();
void initialise_parser_meta();

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

int read_defcount(char* defcount_val);
int read_usecount(char* val);
int read_codecount(char* val);

void update_symbol_table(string symbol);
void update_module_table(int codecount);

void print_symbol_table();
int main()
{
    
    //open file and send filehandler to tokenizer
    string file_name = "input-2";
    ifstream input_file(file_name);
    // First parse
    if(input_file.is_open())
    {
        reset_module();
        initialise_parser_meta();
        printf("starting parse 1");
        parser(1,input_file);
        input_file.close();
    }

    // Print symbol table
    print_symbol_table();
    printf("\nMemory Map\n");

    //  Second Parse
    input_file.open("input-1");
    if(input_file.is_open())
    {
        reset_module();
        initialise_parser_meta();
        printf("starting parse 2");
        parser(2, input_file);
        input_file.close();
    }
}

void parser(int iteration, ifstream &input_file_handler)
{
    int line_num = 1;
    int module_num = 0;
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
                    printf("AT DEFLIST\t");
                    do_at_deflist(token);
                    break;
                case 2:
                    printf("AT USELIST\t");
                    do_at_uselist(token);
                    break;
                case 3:
                    printf("AT PROGRAMTEXT:codecount:%d\t",codecount);
                    do_at_program_text(token);
                    break;
            }
            printf("%s\n", token);
            token = get_token(); //subsequent tokens
        }
        
    }
}

char* get_token()
{
    return strtok(nullptr, delim);
}

bool validate_token(string token_type)
{
    // do a case implementation 
    //     for deflist - symbol, integer
    //     for useline - symbols
    //     for program test - codecount, addrmode, instruction
        return true;
}



// keeping track of where we are during parsings


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
        printf("DEAD");
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
}
void do_at_deflist(char* val){
    if(parser_meta.read_defcount)
    {
        reset_module();
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
            parser_meta.read_usecount = true;
        }
    }

    else if(parser_meta.read_S)
    {
        do_when_reading_s_at_deflist(val);
        parser_meta.read_S = false;
        parser_meta.read_R = true ;
    }

    else if (parser_meta.read_R)
    {
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
        printf("DEAD");
    }
    

}

void do_when_reading_defcount(char* val)
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
            module_list.push_back(0);
        }
        else{
            int prev_module = module_list.back();
            module_list.push_back(prev_module+1);
        }
        
        }
}

void do_when_reading_s_at_deflist(char* val)
{
    string symbol_ = change_string_type(val);
    symbols.push_back(symbol_);
    
}

void do_when_reading_r_at_deflist(char* val)
{
    //validate r
    //update symbol table with rel addr
    string symbol_ = symbols.back();
    int curr_module = module_list.back();
    int curr_base_addr = memory_base_table[curr_module];
    symbol_table[symbol_] =  atoi(val) + curr_base_addr;
    defcount -= 1;
}

void do_at_uselist(char* val){
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
void do_when_reading_usecount(char* val)
{
    //validate(usecount)
    usecount = read_usecount(val);
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
void do_at_program_text(char* val){
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

        }
        
    }
    else if(parser_meta.read_mode)
    {
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
    if(codecount>-1)
    {
        //syntactically ok to read (M,I)
        update_module_table(codecount);
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
    int prev_base_address = memory_base_table[module_num];
    memory_base_table[module_num+1] = codecount + prev_base_address;

}

void print_symbol_table()
{
    printf("Symbol Table\n");
    for (auto it = symbols.begin(); it != symbols.end(); ++it) {
        string symbol_ = *it ;
        int addr_ = symbol_table[symbol_];
        printf("%s=%d\n", symbol_.c_str(),addr_);
    }
}
