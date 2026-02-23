MPIC = mpicc

.PHONY: all clean

all: csq ptp pc

clean:
	rm -f csq ptp pc

csq: consequtive.c
	$(MPIC) $^ -o $@

ptp: parallel-ptp.c
	$(MPIC) $^ -o $@

pc: parallel-collective.c
	$(MPIC) $^ -o $@
