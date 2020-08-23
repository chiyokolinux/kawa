include config.mk

PROGBIN = kawa

PROGOBJ = kawa.o
DFSAOBJ = depresolve.o
HELPOBJ = help.o
INSTOBJ = install.o
REMVOBJ = remove.o
SRCHOBJ = search.o
SHOWOBJ = show.o
UPDTOBJ = update.o
SAUCOBJ = sourcepkg.o
PATCOBJ = patchpkg.o
BINAOBJ = binarypkg.o
METAOBJ = metapkg.o
DTBSOBJ = database.o
REPOOBJ = reposync.o
RPADOBJ = repoadd.o

# OBJECTS = $(PROGOBJ) $(DFSAOBJ) $(DTBSOBJ) $(HELPOBJ) $(INSTOBJ) $(REPOOBJ) $(RPADOBJ) $(UPDTOBJ) $(SHOWOBJ) $(SRCHOBJ)
OBJECTS = $(PROGOBJ) $(DTBSOBJ) $(HELPOBJ) $(REPOOBJ) $(RPADOBJ) $(UPDTOBJ) $(SHOWOBJ) $(SRCHOBJ)
HEADERS = config.h datatypes.h help.h repoadd.h reposync.h database.h update.h search.h show.h
# HEADERS = config.h datatypes.h depresolve.h help.h install.h repoadd.h reposync.h database.h update.h search.h show.h

all: $(PROGBIN)

$(PROGBIN): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(PROGOBJ): $(HEADERS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROGOBJ) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(INITBIN) $(DESTDIR)$(PREFIX)/bin/$(SERVBIN)

dist: clean
	mkdir -p kawa-$(VERSION)
	mkdir -p kawa-$(VERSION)/confs
	cp LICENSE Makefile README config.def.h config.mk kawa.c *.h kawa-$(VERSION)
	tar -cf kawa-$(VERSION).tar kawa-$(VERSION)
	gzip kawa-$(VERSION).tar
	rm -rf kawa-$(VERSION)

clean:
	rm -f $(PROGBIN) *.o kawa-$(VERSION).tar.gz

.SUFFIXES: .def.h

.def.h.h:
	cp $< $@

.PHONY:
	all install uninstall dist clean
