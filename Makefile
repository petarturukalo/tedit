srcs=$(shell find src -name "*.c" -print)
objs=$(patsubst src/%.c, build/%.o, $(srcs))
CC=gcc
CFLAGS=-c -g
LDFLAGS=-lm -lncurses -lpthread

tedit: $(objs)
	$(CC) $^ $(LDFLAGS) -o $@

# All header files reside in the same directory as its corresponding
# .c file, but might not exist for some files, so use wildcard since it 
# expands to nothing if it doesn't exist.
.SECONDEXPANSION:
build/%.o: src/%.c $$(wildcard src/%.h)
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f tedit
	find build -name "*.o" -print -delete

install:
	cp tedit /usr/bin

uninstall:
	rm /usr/bin/tedit
