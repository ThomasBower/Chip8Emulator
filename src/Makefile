CC = gcc
CFLAGS = -Wall -pedantic -lSDL2

.SUFFIXES: .c .o

.PHONY: clean

cpu: cpu.c cpu.h
	$(CC) $(CFLAGS) -o cpu cpu.c

clean:
	rm -f *.o
	rm -f cpu
