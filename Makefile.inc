# Generic variables
CC      = gcc
CFLAGS  = -std=c99 -D_POSIX_C_SOURCE=200112L -I./include -fPIC -Wall -g -O0 -fstack-protector-all
LDFLAGS = -rdynamic
LD      = $(CC)
RM      = rm
ECHO    = echo
MODPATH = modules

LIBS      = -ldl -lJudy
