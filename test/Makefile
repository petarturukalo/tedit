srcs=$(shell find src -name '*.c' -print)
# Test assert object files.
objs=$(patsubst %.c, %.o, $(srcs))
# Objects from text editor.
TEOBJS=../src/ds/dlist.o ../src/math.o ../src/tab.o \
	../src/ds/str.o ../src/chrp.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lpthread -lcurses

# Executable which tests by asserting.
test-assert: $(objs) 
	$(CC) $(objs) $(TEOBJS) $(LFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@


clean:
	find src -name '*.o' -print -delete
	rm test-assert 

