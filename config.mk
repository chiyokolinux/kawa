VERSION = 0.3

PREFIX =
MANPREFIX = $(PREFIX)/share/man

CC = gcc
LD = $(CC)
CPPFLAGS =
CFLAGS   = -Wextra -Wall -Os -g
# LDFLAGS  = -s -lcurl -static
LDFLAGS  = 
LDLIBS   = -lcurl
# I don't wanna install glibc-static
