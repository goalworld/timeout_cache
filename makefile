CC = gcc
SRCS = ./test.c ./entrymaptimout.c
DEBUG=-g
CFALGS = 
test:$(SRCS)
	$(CC) $(DEBUG) $(CFALGS) -o $@ $^ 