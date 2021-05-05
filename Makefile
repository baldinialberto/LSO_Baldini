VERBOSE:=TRUE

CC:=gcc
STRIP:=strip
 
CFLAGS:=-w -O2 
 
.PHONY: all
.DEFAULT: all
 
OBJDIR:=./obj
SRCDIR:=./src
SRCS:=$(wildcard $(SRCDIR)/*.c)
OBJS:=$(patsubst $(SRCDIR)/%.c,%.o,$(SRCS))
 
PROGRAM:=main.elf
 
all: $(PROGRAM)
 
$(OBJS): $(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)
	$(STRIP) $(PROGRAM)
 
clean:
	rm -f $(OBJDIR)/* $(PROGRAM)