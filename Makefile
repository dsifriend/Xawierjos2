CC = gcc
CFLAGS = -std=c11 -w -Wall -Wpedantic -g
LDFLAGS = 

JavierDict: main.c
	$(CC) $(CFLAGS) $? $(LDFLAGS) -o $@

