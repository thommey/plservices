include ../Makefile.inc

MOD = ../lua.so

# Automatic rules
HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# Lua flags
LUAPKG    := $(shell for luaver in lua lua5.2 lua5.1 lua5.0; do pkg-config --exists $$luaver && echo $$luaver && break; done )
LUACFLAGS := $(shell pkg-config --cflags $(LUAPKG))
LUALIBS   := $(shell pkg-config --libs $(LUAPKG))

CFLAGS += $(LUACFLAGS)
LIBS += $(LUALIBS)

all: $(MOD)

$(MOD): $(OBJS)
	$(LD) -shared $(LDFLAGS) $(OBJS) $(LIBS) -o $(MOD)

.PHONY: clean

clean:
	$(RM) -f $(OBJS) $(MOD)
