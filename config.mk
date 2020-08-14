VERSION = 0.1

PREFIX =
MANPREFIX = $(PREFIX)/share/man

CC = gcc
LD = $(CC)
CPPFLAGS =
CFLAGS   = -Wextra -Wall -Os -s
# LDFLAGS  = -s -lcurl -static
LDFLAGS  = -s -lcurl
# I don't wanna install glibc-static
