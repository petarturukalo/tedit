# Test assert object files.
AOBJS=build/test-assert.o build/test-ds.o build/test-tab.o
# Test memory leak object files.
MOBJS=build/test-mem.o
# Objects from text editor for assert object files.
TEAOBJS=../build/ds/slist.o ../build/ds/dlist.o ../build/math.o ../build/tab.o \
	../build/ds/str.o ../build/chrp.o
# Objects from text editor for memory leak object files.
TEMOBJS=../build/tedata.o ../build/fbuf/elbuf.o ../build/fbuf/bufs.o ../build/cmd/cmd.o \
	../build/cmd/cmds.o ../build/cmd/fcmd.o ../build/cmd/acmd.o ../build/math.o \
	../build/fbuf/fbuf.o ../build/chrp.o ../build/fbuf/fbio.o ../build/lines.o \
	../build/ds/slist.o ../build/ds/dlist.o ../build/ds/str.o ../build/tab.o ../build/view.o \
	../build/line.o ../build/cursor.o ../build/fbuf/fbaux.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lpthread -lcurses

all: test-assert test-mem

# Executable which tests by asserting.
test-assert: $(AOBJS) 
	$(CC) $(AOBJS) $(TEAOBJS) $(LFLAGS) -o $@

# Executable which should be tested for memory leaks
# under valgrind.
test-mem: $(MOBJS)
	$(CC) $(MOBJS) $(TEMOBJS) $(LFLAGS) -o $@


build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm test-{assert,mem} build/*.o vgcore*

ctags:
	myctags -R ..
