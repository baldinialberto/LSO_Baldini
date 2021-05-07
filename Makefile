CC:=gcc

.PHONY: all clean

TARGETs:= client.exe server.exe
 
all: $(TARGETs)

server.exe : server.o sockets.o server_utils.o
	$(CC) -o $@	$^

client.exe : client.o sockets.o

%.o : src/%.c include/%.h
	$(CC) $< -c 

clean : 
	-rm *.o *.exe

clean_objs :
	-rm *.o
