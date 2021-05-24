CC:=gcc
CFLAGS:= -Wall -pedantic -I ./include 
LIBS:= -lpthread

.PHONY: all clean no_obj

TARGETS:= client.exe server.exe
LIB:= libsol.so
LIB_OBJS:= 

no_obj : all
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