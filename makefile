CC = gcc
SRCS = ./test/test.c ./src/toentrytable.c 
DEBUG=-g 
CFALGS = -O3 -Wall -g3
INC_DIR = -I./include
all:test-realease test-debug

test-realease:$(SRCS)
	$(CC) $(CFALGS) $(INC_DIR) -o $@ $^ -pg -lwod
test-debug:$(SRCS)
	$(CC) $(DEBUG) $(INC_DIR) -o $@ $^ -pg -lwod
clean:
	rm test-*
