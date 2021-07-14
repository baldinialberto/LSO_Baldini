CC:=gcc
CFLAGS:= -Wall -g -I ./include

.PHONY: all clean

TARGETS:= test.exe

all : $(TARGETS)
	-rm -f *.o

test.exe : main.o sys_utils.o ptr_utils.o
	$(CC) $(CFLAGS) -o $@ $^

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c

clean :
	-rm -f *.o *.exe