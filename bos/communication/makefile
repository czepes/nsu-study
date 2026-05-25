CC = gcc

.PHONY: all clean

all: buffer signal pipe

clean:
	rm -f buffer signal pipe

buffer: buffer.c
	$(CC) $< -o $@

signal: signal.c
	$(CC) $< -o $@

pipe: pipe.c
	$(CC) $< -o $@
