#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string.h>
using namespace std; 
char* get_token(const char* delim);
char* change_string_type(string line);
int main()
{
    // char  mystring[] = " What is   your  name \t ?";
    string line;
    ifstream input_file("input-2");
    while(input_file.is_open() && !input_file.eof())
    {
        //printf("INSIDE");
        getline(input_file, line);
        // cout<<line;
        char* line_ = change_string_type(line);
        //printf("LINE: %s",line_);
        char* token;
        const char* delim = " \t\n";
        //printf("beggining of line");
        token = strtok(line_, delim);
        // printf("%s\n",token);
        while(token!=nullptr && token!=NULL)
        {   
            printf("\ngetting token\t");
            printf("%s\n",token);
            token = get_token(delim);
            

        }
        printf("end line\n");
    }

}

char* get_token(const char* delim)
{
    return strtok(nullptr, delim);
}

char* change_string_type(string line)
{
    //printf("\nchanging string\n");
    char* c_str = new char[line.length() + 1];
    strcpy(c_str, line.c_str());
    return c_str;
}
