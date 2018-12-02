CC=gcc
CFLAGS= -std=gnu99 -Wall
LDLIBS = -lpthread -lm

all: test
test: test.c 
	$(CC) $(CFLAGS) $(LDLIBS) -o test test.c
.PHONY: 
	clean all
clean:
	rm test