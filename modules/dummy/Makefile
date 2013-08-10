include ../Makefile.inc

MOD = ../dummy.so

# Automatic rules
HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(MOD)

$(MOD): $(OBJS)
	$(LD) -shared $(LDFLAGS) $(OBJS) $(LIBS) -o $(MOD)

.PHONY: clean

clean:
	$(RM) -f $(OBJS) $(MOD)
