CC=..\tcc\tcc
CFLAGS= -Os -Wall

black:
	$(CC) $(CFLAGS) vm.c black.c compile.c -o black.exe