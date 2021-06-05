CC:=gcc
CFLAGS:=-Wall -pedantic -g
LIB_OBJS:=mem_utils.o string_utils.o thread_utils.o file_utils.o hash_utils.o array_utils.o list_utils.o
EXEC_OBJ:=tester.o
EXEC_LIST:=tester.exe

.PHONY : all clean debug

debug : all clean

all : $(EXEC_LIST)

$(EXEC_LIST): $(LIB_OBJS) $(EXEC_OBJ)

%.exe : %.o
	$(CC) $(CFLAGS) -o $@  $^ -lpthread

%.o : %.c
	$(CC) $(CFLAGS) $< -c

clean :
	-rm *.o