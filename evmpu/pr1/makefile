CC = g++
CFLAGS = -Wall -std=c++17
SRC = prog.cpp
TARGET_BASE = prog
OPT_LEVELS = -O0 -O1 -O2 -O3 -Os -Ofast -Og

TARGETS = $(foreach opt, $(OPT_LEVELS), $(patsubst -%,%,$(opt)))

all: $(TARGETS)

O%: $(SRC)
	$(CC) $(CFLAGS) -O$* -o $(TARGET_BASE)-$@ $<

clean:
	rm -f $(foreach opt, $(TARGETS), $(TARGET_BASE)-$(opt))

.PHONY: all clean
