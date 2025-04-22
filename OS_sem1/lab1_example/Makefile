CC=gcc
CFLAGS=-g

CC=gcc
CFLAGS=-g
DEPS = 
OBJ = top.o print.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

justdoit: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f justdoit *.o *~
