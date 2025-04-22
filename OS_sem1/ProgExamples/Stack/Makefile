CFLAGS=-g -Wall
#CFLAGS=-g

all:  stack fibo

stack: stack.c
	gcc $(CFLAGS) stack.c -o stack -Wreturn-local-addr

fibo: fibo.c
	gcc $(CFLAGS) fibo.c -o fibo 

clean:
	rm -f stack fibo *~
