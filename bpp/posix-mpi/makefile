GCC = gcc
MPIC = mpicc
FLAGS = -O2 -lm -pthread

ifdef W
	FLAGS += -DLOG_WORKER=1
endif
ifdef RP
	FLAGS += -DLOG_RESPONSER=1
endif
ifdef RQ
	FLAGS += -DLOG_REQUESTER=1
endif
ifdef L
	FLAGS += -DLOG=1
endif

all: balance

balance: balance.c
	$(MPIC) $(FLAGS) -o $@ $^

release:
	$(MAKE)

debug:
	$(MAKE) W=1 RQ=1 RP=1

clean:
	rm -f balance

.PHONY: all release debug clean
