CC	= gcc
CFLAGS	=-g -Wall

all: test

%o: %c
	$(CC) -c $(CFLAGS) $<

test: bignum.o main.o
	$(CC) -o $@ $^

# If the first argument is "run"...
ifeq (check,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  CHECK_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(CHECK_ARGS):;@:)
endif

run:
	./test

.PHONY: check

check:
	./test $(CHECK_ARGS)
	python3 helper/verify.py

clean:
	$(RM) test *.o helper/*.txt

