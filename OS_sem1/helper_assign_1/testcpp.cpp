#include <iostream>
#include <string.h>
#include <vector> 
#include <unordered_map>
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
// 1. Use a unordered map
// 2. key is module number and value is the base address calculated
unordered_map<int, int> memory_base_table;


// FOR where is the parser at used during parse 1 and parse 2
unordered_map<string, bool> parser_at = 
{
    {"at_deflist", false},
    {"at_uselise", false},
    {"at_program_text", false}
};

// FOR delim used in strtok
// using const as delim is independent of any input
const char* delim = " \t\n";

// Each module has defcount, usecount, codecount
// which then tells how many arguments need to be parsed
int defcount, usecount, codecount;

// +++++++++++++++++++++ ALL function definitons +++++++++++++++++++++++
void at_deflist_change();
void at_uselist_change();
void at_program_text_change();
bool is_deflist();
bool is_uselist();
bool is_program_text();
void reset_module();
char* get_token();
bool validate_token(string token_type);
void parser(int iteration, ifstream &input_file_handler);
char* change_string_type(string line);

int main()
{
    
    //open file and send filehandler to tokenizer
    string file_name = "input-1";
    ifstream input_file("input-1");
    // First parse
    if(input_file.is_open())
    {
        printf("starting parse 1");
        parser(1,input_file);
        input_file.close();
    }

    // Print symbol table

    //  Second Parse
    input_file.open("input-1");
    if(input_file.is_open())
    {
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
    int defcount = 0; 
    while(input_file_handler && !input_file_handler.eof())
    {
        string line;
        getline(input_file_handler, line);
        char* line_ = change_string_type(line);
        char* token;
        const char* delim = " \t\n";
       // printf("\nlines start \n");
        token = strtok(line_, delim); //first token of any line in file
        if(line_num==0)
	{
		defcount = token;
	}
	while(token != nullptr)
        {
            printf("%s\t", token);
            token= get_token(); //subsequent tokens
        }
	line_num++;
        
    }
}



    // PARSE 1
    // for each token do a validation
    // after validation create symbol table at deflist
    // do nothing at uselist --> for_now
    // at program text calculate the number of instruction and update memory table


    // PARSE 2
    // at every programtext go through each pair (addrmmode, instruction)
    // acc to addrmode do the change tothe instruction and 
    // PRINT --> MEMORY ++ : NEW INSTRUCTION INT based on MARIE

    

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
void at_deflist_change()
{
    parser_at["at_deflist"] ? parser_at["at_deflist"]=false : parser_at["at_deflist"]=true ;
}

void at_uselist_change()
{
    parser_at["at_uselist"] ? parser_at["at_uselist"]=false : parser_at["at_deflist"]=true ;
}

void at_program_text_change()
{
    parser_at["at_program_text"] ? parser_at["at_program_text"]=false : parser_at["at_program_text"]=true ;
}

bool is_deflist()
{
    return parser_at["at_deflist"];
}

bool is_uselist()
{
    return parser_at["at_deflist"];
}

bool is_program_text()
{
    return parser_at["at_program_text"];
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
