srcs=$(shell find src -name '*.c' -print)
objs=$(patsubst %.c, %.o, $(srcs))
deps=$(patsubst %.o, %.d, $(objs))
CC=gcc
CFLAGS=-c -g
LDLIBS=-lm -lncurses -lpthread

-include $(deps)

tedit: $(objs)
	$(CC) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD $< -o $@


clean:
	find src -name '*.[od]' -print -delete
	rm tedit

install:
	sudo cp -v tedit /usr/bin

uninstall:
	sudo rm -v /usr/bin/tedit

