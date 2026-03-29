CC = gcc

.PHONY: all clean rm

all: reverse manager pagemap

clean:
	rm -f reverse manager pagemap \
		mkdir prndir rmdir \
	  mkfl prnfl rmfl \
		mksl prnsl prnslf rmsl \
		mkhl rmhl \
		prnmod chmod

rm:
	rm -rf 1rid dir1/2rid dir1/dir2/3rid

reverse: reverse.c
	$(CC) $^ -o $@

pagemap: pagemap.c
	$(CC) $^ -o $@

manager: manager.c
	$(CC) $^ -o $@
	ln $@ mkdir
	ln $@ prndir
	ln $@ rmdir
	ln $@ mkfl
	ln $@ prnfl
	ln $@ rmfl
	ln $@ mksl
	ln $@ prnsl
	ln $@ prnslf
	ln $@ rmsl
	ln $@ mkhl
	ln $@ rmhl
	ln $@ prnmod
	ln $@ chmod
