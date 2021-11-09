CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: main

main: main.o
	${CC} main.o -o main

main.o: main.c
	${CC} ${CFLAGS} main.c

clean:
	rm -f *.o *~