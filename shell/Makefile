all: shell

CFLAGS = -g -O2

shell: mytoc.o strlib2.o shell.o
	cc -o shell mytoc.o strlib2.o shell.o
clean:
	rm -f  *.o shell shell.exe
