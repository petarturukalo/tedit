srcs=$(shell find src -name '*.c' -print)
objs=$(patsubst %.c, %.o, $(srcs))
deps=$(patsubst %.o, %.d, $(objs))
CC=gcc
CFLAGS=-c -g
LDLIBS=-lm -lncurses -lpthread

tedit: $(objs)
	$(CC) $^ $(LDLIBS) -o $@

-include $(deps)

%.o: %.c
	$(CC) $(CFLAGS) -MMD $< -o $@


clean:
	find src -name '*.[od]' -print -delete
	rm tedit

