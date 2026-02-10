CC = gcc
CFLAGS = -Wall -O1 -march=native
TARGETS = main

.PHONY: all clean

all: $(TARGETS)

main: main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS) $(OBJ)
