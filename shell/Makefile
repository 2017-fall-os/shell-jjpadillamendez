all: shell

CFLAGS = -g -O2

shell: mytoc.o strlib2.o shell.o pipeLib.o vectorLib.o
	cc -o shell mytoc.o strlib2.o shell.o pipeLib.o vectorLib.o
clean:
	rm -f  *.o shell shell.exe
