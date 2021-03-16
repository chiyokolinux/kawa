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
PTCHOBJ = patchpkg.o
PATCOBJ = patchpkg.o
BINAOBJ = binarypkg.o
METAOBJ = metapkg.o
DTBSOBJ = database.o
REPOOBJ = reposync.o
RPADOBJ = repoadd.o
MKPGOBJ = makepackages.o
KWFLOBJ = kawafile.o
CACLOBJ = cacheclean.o
EERROBJ = error.o

OBJECTS = $(PROGOBJ) $(BINAOBJ) $(CACLOBJ) $(DFSAOBJ) $(DTBSOBJ) $(EERROBJ) $(HELPOBJ) $(INSTOBJ) $(KWFLOBJ) $(METAOBJ) $(MKPGOBJ) $(PTCHOBJ) $(REMVOBJ) $(REPOOBJ) $(RPADOBJ) $(UPDTOBJ) $(SAUCOBJ) $(SHOWOBJ) $(SRCHOBJ)
HEADERS = config.h binarypkg.h cacheclean.h datatypes.h depresolve.h error.h help.h install.h kawafile.h makepackages.h metapkg.h patchpkg.h remove.h repoadd.h reposync.h database.h update.h search.h show.h sourcepkg.h

all: $(PROGBIN)

$(PROGBIN): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(PROGOBJ): $(HEADERS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROGBIN) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROGBIN)

dist: clean
	mkdir -p kawa-$(VERSION)
	cp LICENSE Makefile README config.mk *.c *.h kawa-$(VERSION)
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
