#include <stdio.h>

// references to other modules
extern void print_hello(int);
extern int myvalue;

int x = 0;   

void
local_hello()
{
	printf("local hello\n");
}

int
main(int argc, char **argv)
{
	print_hello(myvalue);
	local_hello(myvalue);
        x = 1;
	print_hello(myvalue);
	return 0;
}
