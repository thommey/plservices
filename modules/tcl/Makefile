include ../Makefile.inc

MOD = ../tcl.so

# Automatic rules
HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# TCL flags
TCLPKG    := $(shell for tclver in tcl tcl8.6 tcl8.5; do pkg-config --exists $$tclver && echo $$tclver && break; done )
TCLCFLAGS := $(shell pkg-config --cflags $(TCLPKG))
TCLLIBS   := $(shell pkg-config --libs $(TCLPKG)) -ltcl$(shell pkg-config --modversion $(TCLPKG))

CFLAGS += $(TCLCFLAGS)
LIBS += $(TCLLIBS)

all: $(MOD)

$(MOD): $(OBJS)
	$(LD) -shared $(LDFLAGS) $(OBJS) $(LIBS) -o $(MOD)

.PHONY: clean

clean:
	$(RM) -f $(OBJS) $(MOD)
