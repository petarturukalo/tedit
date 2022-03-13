# TODO try to automate collecting these object files with something
# like gcc -M
#
# Test assert object files.
AOBJS=build/test-assert.o build/test-ds.o build/test-tab.o
# Objects from text editor for assert object files.
TEAOBJS=../build/ds/dlist.o ../build/math.o ../build/tab.o \
	../build/ds/str.o ../build/chrp.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lpthread -lcurses

# Executable which tests by asserting.
test-assert: $(AOBJS) 
	$(CC) $(AOBJS) $(TEAOBJS) $(LFLAGS) -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm test-assert build/*.o

