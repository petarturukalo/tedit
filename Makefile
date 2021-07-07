OBJS=build/lines.o build/tedit.o build/fbuf/fbuf.o build/fbuf/fbaux.o build/tedata.o build/input.o \
	build/display.o build/main.o build/cursor.o build/tab.o build/view.o build/line.o build/linsert.o \
	build/move.o build/tedit.o build/fbuf/fbinp.o build/fbuf/fbio.o build/fbuf/bufs.o \
	build/fbuf/elbuf.o build/fbuf/elinp.o build/cmd/cmd.o build/cmd/fcmd.o build/cmd/cparse.o \
	build/cmd/cmds.o build/cmd/acmd.o build/sig.o build/getch.o build/ds/slist.o build/ds/dlist.o \
	build/ds/str.o build/chrp.o build/math.o
CC=gcc
CFLAGS=-c 
LFLAGS=-lm -lcurses -lpthread

tedit: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f vgcore* tedit
	find build -name "*.o" -print -delete

ctags:
	myctags -R . /usr/include/sys/uio.h /usr/include/curses.h /usr/include/search.h


install:
	cp tedit /usr/local/bin

uninstall:
	rm /usr/local/bin/tedit
