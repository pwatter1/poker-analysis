CC=gcc

all: poker
poker: poker.o
poker.o: poker.c poker.h

clean:
	rm -f poker poker.o
run: poker
	./poker
