CC = gcc

.PHONY: all clean

all: vars malloc env exec overflow prot

clean:
	rm -f vars malloc env exec overflow prot

vars: vars.c
	$(CC) $< -o $@

malloc: buffer.c
	$(CC) $< -o $@

env: env.c
	$(CC) $< -o $@

exec: exec.c
	$(CC) $< -o $@

overflow: overflow.c
	$(CC) $< -o $@

prot: prot.c
	$(CC) $< -o $@
