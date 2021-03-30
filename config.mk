# this file is part of kawa
# Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
#
# kawa is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# kawa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with kawa.  If not, see <https://www.gnu.org/licenses/>.

VERSION = 0.3

PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

CC = gcc
LD = $(CC)
CPPFLAGS =
CFLAGS   = -Wextra -Wall -Os -g
# LDFLAGS  = -s -lcurl -static
LDFLAGS  = 
LDLIBS   = -lcurl
# I don't wanna install glibc-static
