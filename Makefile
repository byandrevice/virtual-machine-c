CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

.PHONY: all run clean
all: vm

vm: vm.c
	$(CC) $(CFLAGS) -o $@ $<

run: vm
	./vm $(INPUT)

clean:
	rm -f vm

