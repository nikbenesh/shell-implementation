CC = gcc
CFLAGS = -Wall

all:mysh
	$(CC) $(CFLAGS) -o mysh mysh.c