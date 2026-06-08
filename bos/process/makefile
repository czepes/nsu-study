CC = gcc

.PHONY: all clean

all: lifecycle reaper stack

clean:
	rm -f lifecycle reaper stack stack.dump hello.asm

lifecycle: lifecycle.c
	$(CC) $< -o $@

reaper: reaper.c
	$(CC) $< -o $@

stack: stack.c
	$(CC) $< -o $@
	objdump -d stack | grep -A30 "<hello>:" > hello.asm
