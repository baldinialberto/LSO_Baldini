CC:=gcc
CFLAGS:=-Wall -pedantic -g
LIB_OBJS:=mem_utils.o string_utils.o thread_utils.o hash_utils.o array_utils.o list_utils.o file_utils.o

.PHONY : lib clean clean_lib

lib : $(LIB_OBJS)
	$(CC) -shared -o libso.so $^
	-rm *.o

%.o : %.c
	$(CC) $(CFLAGS) $< -c -fPIC

clean :
	-rm -f *.o

clean_lib :
	-rm -f *.so