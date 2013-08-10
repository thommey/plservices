# Generic variables
CC      = gcc
CFLAGS  = -std=c99 -O0 -g -Wall -I./include -fPIC
LDFLAGS = -rdynamic
LD      = $(CC)
RM      = rm
ECHO    = echo
MODPATH = modules

LIBS      = -ldl -lJudy

# Lua flags
LUAPKG    := $(shell for luaver in lua lua5.2 lua5.1 lua5.0; do pkg-config --exists $$luaver && echo $$luaver && break; done )
LUACFLAGS := $(shell pkg-config --cflags $(LUAPKG))
LUALIBS   := $(shell pkg-config --libs $(LUAPKG))

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $(LUACFLAGS) $< -o $@
