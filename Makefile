CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
LD=gcc
LIBS=-lJudy -llua

HDRS = $(wildcard src/*.h)
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = plservices

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXEC)

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXEC) $(OBJS) *.i *.s
