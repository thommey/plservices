# Generic variables
CC      = gcc
CFLAGS  = -std=c99 -O0 -g -Wall -I./include -fPIC
LDFLAGS = -rdynamic
LD      = $(CC)
RM      = rm
ECHO    = echo
MODPATH = modules

LIBS      = -ldl -lJudy

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) $< -o $@
