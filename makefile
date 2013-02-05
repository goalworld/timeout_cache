CC = gcc
SRCS = ./test.c ./entrymaptimout.c
DEBUG=-g
CFALGS = -O3
test:$(SRCS)
	$(CC) $(DEBUG) $(CFALGS) -o $@ $^ 