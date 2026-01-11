CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CLANG = clang
FUZZ_FLAGS = -g -O1 -fsanitize=fuzzer,address,undefined -DSAFE_SEARCH_LIB

.PHONY: all test fuzz clean

all: test

# Standard Verification (Unit Tests + Property Based)
test: safesearch
	./safesearch

safesearch: safesearch.c safesearch.h
	$(CC) $(CFLAGS) -o safesearch safesearch.c

# Fuzz Testing (Requires clang)
fuzz: fuzz_safesearch
	./fuzz_safesearch -max_total_time=60

fuzz_safesearch: safesearch.c safesearch.h fuzz_safesearch.c
	$(CLANG) $(FUZZ_FLAGS) safesearch.c fuzz_safesearch.c -o fuzz_safesearch

clean:
	rm -f safesearch fuzz_safesearch *.o
