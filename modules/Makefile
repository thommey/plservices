include ../Makefile.inc

DIRS := $(shell find . -mindepth 1 -maxdepth 1 -type d -print)

all: $(DIRS)
	
$(DIRS):
	$(ECHO) Making module $@; $(MAKE) $(MFLAGS) -C $@

clean:
	for d in $(DIRS); do $(MAKE) -C $$d clean; done

.PHONY: $(DIRS) clean
