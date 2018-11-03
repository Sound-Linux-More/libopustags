PNAME=opustags
DESTDIR=/usr/local
SRCDIR=src
DOCDIR=doc
MANDEST=share/man
DOCDEST=share/doc
VERSO=1
VER=1.2.3
PLIBDEV=lib$(PNAME).so
PLIB=$(PLIBDEV).$(VERSO)
PLIBFULL=$(PLIBDEV).$(VER)
CC=gcc
CFLAGS=-Wall
LDFLAGS=-logg -s
INSTALL=install
GZIP=gzip
RM=rm -f
LN=ln -fs
CHMOD=chmod 644

all: $(PNAME) man

$(PLIB): $(SRCDIR)/$(PNAME).c $(SRCDIR)/picture.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@ $^ -o $(PLIBFULL)
	$(CHMOD) $(PLIBFULL)
	$(LN) $(PLIBFULL) $@
	$(LN) $@ $(PLIBDEV)

$(PNAME): $(PLIB) $(SRCDIR)/tool.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

man: $(DOCDIR)/$(PNAME).1
	$(GZIP) < $^ > $(PNAME).1.gz

install: $(PNAME) man
	$(INSTALL) -d $(DESTDIR)/bin
	$(INSTALL) -m 755 $(PNAME) $(DESTDIR)/bin/
	$(INSTALL) -d $(DESTDIR)/include/opus
	$(INSTALL) -m 0644 $(SRCDIR)/$(PNAME).h $(DESTDIR)/include/opus/$(PNAME).h
	$(INSTALL) -d $(DESTDIR)/lib
	$(INSTALL) -m 0644 $(PLIBFULL) $(DESTDIR)/lib/$(PLIBFULL)
	$(LN) $(PLIBFULL)  $(DESTDIR)/lib/$(PLIB)
	$(LN) $(PLIB) $(DESTDIR)/lib/$(PLIBDEV)
	$(INSTALL) -d $(DESTDIR)/$(MANDEST)/man1
	$(INSTALL) -m 644 $(PNAME).1.gz $(DESTDIR)/$(MANDEST)/man1/
	$(INSTALL) -d $(DESTDIR)/$(DOCDEST)/$(PNAME)
	$(INSTALL) -m 644 $(DOCDIR)/sample_audiobook.opustags $(DOCDIR)/AUTHORS $(DOCDIR)/CHANGELOG LICENSE README.md $(DESTDIR)/$(DOCDEST)/$(PNAME)

uninstall:
	$(RM) $(DESTDIR)/bin/$(PNAME)
	$(RM) $(DESTDIR)/lib/$(PLIBDEV)*
	$(RM) $(DESTDIR)/$(MANDEST)/man1/$(PNAME).1.gz
	$(RM) -r $(DESTDIR)/$(DOCDEST)/$(PNAME)

clean:
	$(RM) $(PNAME) $(PLIBDEV)* $(PNAME).1.gz
