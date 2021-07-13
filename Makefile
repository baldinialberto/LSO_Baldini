CC:=gcc
CFLAGS:= -Wall -pedantic -g -I ./include -I ./libs/LSO_LIB
LIBS:= -lpthread -L ./libs/LSO_LIB -lso -Wl,-rpath,./libs/LSO_LIB
VPATH:=src:libs/LSO_LIB

.PHONY: all clean lib

TARGETS:= client.exe server.exe


all : lib $(TARGETS)
	cd to_write && bash ./autogen.sh && cd ../
	-rm -f *.o
	cd libs/LSO_LIB && make clean && cd ../../

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
	-rm -f to_write/*.txt
	-rm -r -f readen
	-rm -r -f evicted
	cd libs/LSO_LIB && make clean && make clean_lib && cd ../../