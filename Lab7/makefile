CC=gcc
all: run
run: server.c client.c
	$(CC) -pthread -o server server.c
	$(CC) -o client client.c