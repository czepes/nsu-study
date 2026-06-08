CC = gcc

.PHONY: all clean heap

all: vars malloc env exec overflow prot heap

clean:
	rm -f vars malloc env exec overflow prot \
	heap.o heaptest heapread heap.dump

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

heap.o: heap.c
	$(CC) -c -g -o $@ $<

heapread: heapread.c heap.o
	$(CC) -g -o $@ $^

heaptest: heaptest.c heap.o
	$(CC) -g -o $@ $^

heap: heaptest heapread
