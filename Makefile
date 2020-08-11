include config.mk

PROGBIN = kawa

PROGOBJ = kawa.o
DFSAOBJ = dfs.o
INSTOBJ = install.o
REMVOBJ = remove.o
SRCHOBJ = search.o
SHOWOBJ = show.o
UPDTOBJ = update.o
UPGROBJ = upgrade.o
SAUCOBJ = sourcepkg.o
PATCOBJ = patchpkg.o
BINAOBJ = binarypkg.o
METAOBJ = metapkg.o
DTBSOBJ = database.o
REPOOBJ = reposync.o

all: $(INITBIN) $(SERVBIN) confs

$(INITBIN): $(INITOBJ)
	$(CC) $(LDFLAGS) -o $@ $(INITOBJ) $(LDLIBS)

$(SERVBIN): $(SERVOBJ)
	$(CC) $(LDFLAGS) -o $@ $(SERVOBJ) $(LDLIBS)

$(INITOBJ): config.h
$(SERVOBJ): config.h

confs:
    mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(CONFS) $(DESTDIR)$(PREFIX)/bin

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(INITBIN) $(DESTDIR)$(PREFIX)/bin
	cp -f $(SERVBIN) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(INITBIN) $(DESTDIR)$(PREFIX)/bin/$(SERVBIN)

dist: clean
	mkdir -p ichirou-$(VERSION)
	mkdir -p ichirou-$(VERSION)/confs
	cp LICENSE Makefile README config.def.h config.mk ichirou.c kanrisha.c ichirou-$(VERSION)
	cp confs/rc.conf confs/rc.init confs/rc.local confs/rc.postinit \
	   confs/rc.shutdown ichirou-$(VERSION)/confs
	tar -cf ichirou-$(VERSION).tar ichirou-$(VERSION)
	gzip ichirou-$(VERSION).tar
	rm -rf ichirou-$(VERSION)

clean:
	rm -f $(INITBIN) $(SERVBIN) $(INITOBJ) $(SERVOBJ) ichirou-$(VERSION).tar.gz

.SUFFIXES: .def.h

.def.h.h:
	cp $< $@

.PHONY:
	all install uninstall dist clean
