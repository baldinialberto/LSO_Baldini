CC:=gcc
CFLAGS:= -Wall -I ./include

.PHONY: all clean

TARGETS:= test.exe server.exe

debug : CFLAGS += -D DEBUG -g
debug : all

all : $(TARGETS)
	-rm -f *.o

test.exe : main.o sys_utils.o ptr_utils.o
	$(CC) $(CFLAGS) -o $@ $^

server.exe : server.o sys_utils.o ptr_utils.o
	$(CC) $(CFLAGS) -o $@ $^

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c

clean :
	-rm -f *.o *.exe