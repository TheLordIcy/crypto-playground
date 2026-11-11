# Crypto Playground build
# See Roadmap.md for the overall plan.

CC      = gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Werror -pedantic
LDFLAGS ?=

CORE_SRCS := core/bignum.c core/modmath.c core/primes.c core/rsa.c \
             core/dh.c core/sha256.c core/ecc.c

# Native library built from core/ for testing and benchmarking.
LIB := libcrypto.a

.PHONY: all test bench asan clean wasm

all: $(LIB)

$(LIB): $(CORE_SRCS)
	$(CC) $(CFLAGS) -c core/bignum.c   -o bignum.o
	$(CC) $(CFLAGS) -c core/modmath.c  -o modmath.o
	$(CC) $(CFLAGS) -c core/primes.c   -o primes.o
	$(CC) $(CFLAGS) -c core/rsa.c      -o rsa.o
	$(CC) $(CFLAGS) -c core/dh.c       -o dh.o
	$(CC) $(CFLAGS) -c core/sha256.c   -o sha256.o
	$(CC) $(CFLAGS) -c core/ecc.c      -o ecc.o
	ar rcs $(LIB) bignum.o modmath.o primes.o rsa.o dh.o sha256.o ecc.o

test: $(LIB)
	$(CC) $(CFLAGS) test/run_tests.c $(LIB) -o test/run_tests $(LDFLAGS)
	./test/run_tests

bench: $(LIB)
	$(CC) $(CFLAGS) -O2 test/bench.c $(LIB) -o test/bench $(LDFLAGS)
	./test/bench

asan: CFLAGS += -fsanitize=address,undefined -g -O1
asan: clean
	$(CC) $(CFLAGS) $(CORE_SRCS) test/run_tests.c -o test/run_tests_asan $(LDFLAGS) -fsanitize=address,undefined
	./test/run_tests_asan

wasm:
	emcc core/*.c wasm/bindings.c -O2 -o web/crypto.js \
	  -s EXPORTED_FUNCTIONS='["_malloc","_free"]' \
	  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue"]'

clean:
	rm -f *.o $(LIB) test/run_tests test/run_tests_asan test/bench web/crypto.js web/crypto.wasm
