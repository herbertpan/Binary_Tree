CC=g++
CFLAGS=-std=gnu++0x -Wall -Werror -pedantic

all: test

test: test.o bst.o ; $(CC) $(CFLAGS) test.o bst.o -o test

test.o: test.cpp ; $(CC) $(CFLAGS) -c test.cpp

bst.o: bst.cpp ; $(CC) $(CFLAGS)  -c bst.cpp

clean:;        rm *.o test
