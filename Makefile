CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include -I ./libs/LSO_LIB
LIBS:= -lpthread -L ./libs/LSO_LIB -lso -Wl,-rpath,./libs/LSO_LIB

.PHONY: all clean lib

TARGETS:= client.exe server.exe


all : lib $(TARGETS)

lib :
	cd libs/LSO_LIB && make lib && cd ../../

server.exe : server.o poll_utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
client.exe : client.o dir_utils.o serverapi.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o : src/%.c
	$(CC) $(CFLAGS) $< -c 

clean : 
	-rm -f *.o *.exe *.socket *.log
	cd libs/LSO_LIB && make clean && make clean_lib && cd ../../