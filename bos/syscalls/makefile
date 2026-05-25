TRACE_FLAGS = -N
.PHONY: all clean

all: hello uni sys asm wget trace

clean:
	rm -f hello uni sys \
		hello-trace uni-trace sys-trace \
		curl-trace wget-trace index*.html \
		nolibs nolibs-trace \
		asm asm-trace trace sysinfo.o

hello: hello.c
	gcc $^ -o $@
	strace $(TRACE_FLAGS) -o $@-trace ./$@

uni: unihello.c
	gcc $^ -o $@
	strace $(TRACE_FLAGS) -o $@-trace ./$@

sys: syshello.c
	gcc $^ -o $@
	strace $(TRACE_FLAGS) -o $@-trace ./$@

asm: asmhello.c
	gcc $^ -o $@
	strace $(TRACE_FLAGS) -o $@-trace ./$@

sysinfo.o: sysinfo.c
	gcc $< -c -o $@

trace: trace.c sysinfo.o
	gcc $^ -o $@

nolibs: nolibs.c
	gcc $^ -nostdlib -o $@
	strace $(TRACE_FLAGS) -o $@-trace ./$@

curl:
	strace -C -o curl-trace curl https://kernel.org

wget:
	strace -C -o wget-trace wget https://kernel.org
