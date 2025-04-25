SRCS = $(wildcard src/*.c)
HDRS = $(wildcard src/*.h)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Wextra -Werror -g

.PHONY: all clean

all: sophia

src/parse.c: src/parse.y
	bison -d --locations -o src/parse.c src/parse.y

%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

sophia: src/parse.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) sophia
