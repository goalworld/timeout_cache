CC = gcc
SRCS = ./test.c ./toentrytable.c
DEBUG=-g 
CFALGS = -O3
all:test-realease test-debug

test-realease:$(SRCS)
	$(CC) $(CFALGS) -o $@ $^ -pg
test-debug:$(SRCS)
	$(CC) $(DEBUG) -o $@ $^ -pg