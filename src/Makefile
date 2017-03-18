CC = gcc
CFLAGS = -Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: clean

cpu: cpu.c cpu.h
	$(CC) $(CFLAGS) -o cpu cpu.c

clean:
	rm -f *.o
	rm -f cpu
