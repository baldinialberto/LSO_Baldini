CC:=gcc
CFLAGS:=-Wall -pedantic -I ./include
TARGETS= server.exe client.exe

.PHONY : all

all : $(TARGETS)

server.exe : server.o
    