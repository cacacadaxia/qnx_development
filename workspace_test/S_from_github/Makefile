# Makefile for RTS Project
CC=gcc
CFLAGS=-Wall -pedantic -Wno-char-subscripts -g
CSRC=$(wildcard *.c)

all: controller intersection
	
controller: controller-ui.c msg.c timing.c util.c
	$(CC) $(CFLAGS) -o $@ $^
	
intersection: intersection-ui.c msg.c timing.c util.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o controller intersection core

