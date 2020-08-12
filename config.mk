VERSION = 0.1

PREFIX =
MANPREFIX = $(PREFIX)/share/man

CC = gcc
LD = $(CC)
CPPFLAGS =
CFLAGS   = -Wextra -Wall -Os -s
# LDFLAGS  = -s -static
LDFLAGS  = -s
# I don't wanna install glibc-static
