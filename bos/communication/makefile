CC = gcc

.PHONY: all clean

all: buffer signal pipe

clean:
	rm -f buffer signal pipe \
		server client shutdown

buffer: buffer.c
	$(CC) $< -o $@

signal: signal.c
	$(CC) $< -o $@

pipe: pipe.c
	$(CC) $< -o $@

sock: server.c client.c shutdown.c
	$(CC) -o server server.c
	$(CC) -o client client.c
	$(CC) -o shutdown shutdown.c
