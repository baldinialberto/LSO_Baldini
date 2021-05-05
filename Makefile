CC:=gcc
 
.PHONY: all clean

TARGET:=test.exe
 
all: $(TARGET)

test.exe : main.o strings.o
	$(CC) -o $@	$^

%.o : src/%.c
	$(CC) $^ -c

clean : 
	-rm *.o *.exe

clean_objs :
	-rm *.o