SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Wextra -Werror

.PHONY: all clean

all: parser.c sophia

parser.c: src/sophia.y
	bison -d -o src/parser.c src/sophia.y

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

sophia: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) sophia
