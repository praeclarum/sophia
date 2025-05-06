SRCS = $(wildcard src/*.c)
HDRS = $(wildcard src/*.h)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Wextra -Werror -g

.PHONY: all clean run test accept

all: sophia

src/parse.c: src/parse.y
	bison -d --locations -o src/parse.c src/parse.y

%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

sophia: src/parse.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) sophia
	rm -rf tests/results

run: sophia
	@./sophia

test: sophia
	@mkdir -p tests/results
	@mkdir -p tests/accepted
	./sophia tests/circuit.sophia -o tests/results/circuit.ast -o tests/results/circuit.swift -o tests/results/circuit.cs -o tests/results/circuit.js
	@diff -rq tests/accepted tests/results
	@echo "\033[0;32m✓ outputs match\033[0m"

accept:
	@mkdir -p tests/accepted
	@cp -a tests/results/* tests/accepted/
	@echo "\033[0;32m✓ accepted\033[0m"
