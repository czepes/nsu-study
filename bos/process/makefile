CC = gcc

.PHONY: all clean

all: lifecycle reaper

clean:
	rm -f lifecycle reaper

lifecycle: lifecycle.c
	$(CC) $< -o $@

reaper: reaper.c
	$(CC) $< -o $@
