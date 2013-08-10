include ../Makefile.inc

DIRS := $(shell find . -type d -mindepth 1 -maxdepth 1 -print)

all: $(DIRS)
	
$(DIRS):
	$(ECHO) Making module $@; $(MAKE) $(MFLAGS) -C $@

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean); done

.PHONY: $(DIRS) clean
