include Makefile.inc

# Automatic rules
HDRS = $(wildcard include/*.h)
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = plservices

all: $(EXEC) modules

$(EXEC): $(OBJS) $(HDRS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) $(LUALIBS) -o $(EXEC)

.PHONY: clean modules

clean:
	$(RM) -f $(EXEC) $(OBJS) *.i *.s
	cd $(MODPATH); $(MAKE) $(MFLAGS) clean

modules:
	$(ECHO) Making modules...
	cd $(MODPATH); $(MAKE) $(MFLAGS)

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $< -o $@
