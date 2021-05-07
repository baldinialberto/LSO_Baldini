CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include

.PHONY: all clean no_obj

TARGETS:= client.exe server.exe
LIB:= libsol.so
LIB_OBJS:= server_utils.o string_utils.o commonio.o sockets.o

no_obj : all
	-rm *.o

all: $(TARGETS)

server.exe : server.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	

client.exe : client.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c 

clean : 
	-rm *.o *.exe

