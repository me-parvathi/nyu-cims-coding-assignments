#include <stdio.h>

int myvalue = 1;   // global definition

void 
print_hello(int value)
{
	printf("Hello World %d\n", value);
	myvalue++;   // global definition
}
