CC= gcc
CFLAGS= -std=c99 -O0 -g -Wall -I./include -fPIC
LDFLAGS= -rdynamic
LD= gcc

LIBS = -ldl -lJudy
LUAPKG := $(shell for luaver in lua lua5.2 lua5.1 lua5.0; do pkg-config --exists $$luaver && echo $$luaver && break; done )
LUACFLAGS := $(shell pkg-config --cflags $(LUAPKG))
LUALIBS := $(shell pkg-config --libs $(LUAPKG))

HDRS = $(wildcard src/*.h)
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = plservices

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) $(LUALIBS) -o $(EXEC)

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $(LUACFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXEC) $(OBJS) *.i *.s

modules:
	$(CC) -c $(CFLAGS) $(LUACFLAGS) $(wildcard src/modules/*.c)
	$(CC) -g -shared -Wl,-soname,src/modules/dummy.so.1 -o src/modules/dummy.so.1.0 *.o
	ln -sf src/modules/dummy.so.1.0 src/modules/dummy.so.1
	ln -sf src/modules/dummy.so.1.0 src/modules/dummy.so
