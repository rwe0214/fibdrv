CC	= gcc
CFLAGS	=-g -Wall

all: test

%o: %c
	$(CC) -c $(CFLAGS) $<

test: bignum.o main.o
	$(CC) -o $@ $^

run:
	./test

clean:
	$(RM) test *.o

