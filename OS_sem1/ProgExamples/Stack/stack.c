#include <stdio.h>

int* f1()
{
    int lval;
    printf("%s: %d\n", __FUNCTION__, lval);
    lval = 10;
    return &lval;
}

int f2()
{
    int lval;
    lval = 20;
    return lval;
}

int main()
{
    int *pi;

    pi = f1();
    printf("pi=%p val=%d\n",pi, *pi);
    f2();
    printf("pi=%p val=%d\n",pi, *pi);
    pi = f1();
    printf("pi=%p val=%d\n",pi, *pi);
    return 0;
}



    

