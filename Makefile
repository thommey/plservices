CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
LD=gcc

HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
EXEC = plservices

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXEC) -lJudy

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXEC) $(OBJS)
