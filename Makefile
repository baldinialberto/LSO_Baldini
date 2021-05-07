CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include

.PHONY: all clean no_obj

TARGETS:= client.exe server.exe

no_obj : all
	-rm *.o

all: $(TARGETS)

server.exe : server.o sockets.o server_utils.o commonio.o
	$(CC) $(CFLAGS) -o $@ $^
	

client.exe : client.o sockets.o
	$(CC) $(CFLAGS) -o $@ $^
	

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c 

clean : 
	-rm *.o *.exe

