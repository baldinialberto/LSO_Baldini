CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include 
LIBS:= -lpthread

.PHONY: all clean no_obj test

TARGETS:= client.exe server.exe
LIB:= libsol.so
LIB_OBJS:= mem_utils.o string_utils.o list_utils.o hash_utils.o file_utils.o serverapi.o poll_utils.o dir_utils.o

no_obj : all $(LIB_OBJS)
	-rm *.o

all: $(TARGETS)

server.exe : server.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
client.exe : client.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o : src/%.c include/%.h
	$(CC) $(CFLAGS) $< -c 

clean : 
	-rm *.o *.exe *.socket *.log