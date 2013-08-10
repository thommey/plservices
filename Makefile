include Makefile.inc

# Lua flags
LUAPKG    := $(shell for luaver in lua lua5.2 lua5.1 lua5.0; do pkg-config --exists $$luaver && echo $$luaver && break; done )
LUACFLAGS := $(shell pkg-config --cflags $(LUAPKG))
LUALIBS   := $(shell pkg-config --libs $(LUAPKG))

# Automatic rules
HDRS = $(wildcard src/*.h)
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = plservices

all: $(EXEC) modules

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) $(LUALIBS) -o $(EXEC)

.PHONY: clean modules

clean:
	$(RM) -f $(EXEC) $(OBJS) *.i *.s
	cd $(MODPATH); $(MAKE) $(MFLAGS) clean

modules:
	$(ECHO) Making modules...
	cd $(MODPATH); $(MAKE) $(MFLAGS)
