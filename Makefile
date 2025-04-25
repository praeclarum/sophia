SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Wextra -Werror

.PHONY: all clean

all: sophia

parse.c: src/parse.y
	bison -d --locations -o src/parse.c src/parse.y

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

sophia: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) sophia
