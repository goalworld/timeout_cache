CC = gcc
SRCS = ./test/test.c ./src/toentrytable.c ./src/hashmap.c
DEBUG=-g 
CFALGS = -O3 -Wall -g3
INC_DIR = -I./include
all:test-realease test-debug hash_test

test-realease:$(SRCS)
	$(CC) $(CFALGS) $(INC_DIR) -o $@ $^ -pg
test-debug:$(SRCS)
	$(CC) $(DEBUG) $(INC_DIR) -o $@ $^ -pg
hash_test:./src/toentrytable.c ./src/hashmap.c ./test/hashmap_test.c
	$(CC) $(CFALGS) $(INC_DIR) -o $@ $^ -pg
clean:
	rm test-* hash_test*
