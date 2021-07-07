CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include -I ./libs/LSO_LIB
LIBS:= -lpthread -L ./libs/LSO_LIB -lso -Wl,-rpath,./libs/LSO_LIB

.PHONY: all clean no_obj test

TARGETS:= client.exe server.exe


no_obj : all $(LIB_OBJS)
	-rm *.o

all: $(TARGETS)

server.exe : server.o poll_utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
client.exe : client.o dir_utils.o serverapi.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c 

clean : 
	-rm *.o *.exe *.socket *.log