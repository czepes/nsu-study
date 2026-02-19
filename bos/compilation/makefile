.PHONY: all clean

all: hello hello-static hello-dynamic hello-runtime

clean:
	rm -f hello hello.i hello.s hello.o \
		hello-static hello-static.o \
		./static/static-hello.o ./static/static-hello.a \
		hello-dynamic ./dynamic/libdynamic.so \
		hello-runtime ./runtime/libruntime.so

hello: hello.c
	gcc hello.c -o hello

hello-verbose: hello.c
	gcc hello.c -save-temps -v -o hello

hello-static: hello-static.c ./static/static-hello.c
	gcc ./static/static-hello.c -c -o ./static/static-hello.o
	ar rc ./static/static-hello.a ./static/static-hello.o
	gcc hello-static.c -c -o hello-static.o
	gcc hello-static.o ./static/static-hello.a -o hello-static

hello-dynamic: hello-dynamic.c ./dynamic/dynamic-hello.c
	gcc ./dynamic/dynamic-hello.c -shared -o ./dynamic/libdynamic.so
	gcc hello-dynamic.c -L./dynamic/ -ldynamic -o hello-dynamic

hello-runtime: hello-runtime.c
	gcc ./runtime/runtime-hello.c -shared -o ./runtime/libruntime.so
	gcc hello-runtime.c -o hello-runtime
