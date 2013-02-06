CC = gcc
SRCS = ./test.c ./toentrytable.c
DEBUG=-g
CFALGS = -O3
test-realease:$(SRCS)
	$(CC) $(CFALGS) -o $@ $^
test-debug:$(SRCS)
	$(CC) $(DEBUG) $(CFALGS) -o $@ $^ 
 