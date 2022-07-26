ACLOCAL_AMFLAGS = -I m4

DEFS += -DRANDOM_PREFIX=libopustags

AM_CFLAGS = -I$(top_srcdir)/include $(DEPS_CFLAGS)

dist_doc_DATA = LICENSE doc/AUTHORS doc/CHANGELOG doc/sample_audiobook.opustags README.md

opusincludedir = ${includedir}/opus
opusinclude_HEADERS = src/opustags.h

lib_LTLIBRARIES = libopustags.la

libopustags_la_SOURCES = \
	src/opustags.c \
	src/picture.c
libopustags_la_LIBADD = $(DEPS_LIBS)
libopustags_la_LDFLAGS = -no-undefined \
 -version-info @OP_LT_CURRENT@:@OP_LT_REVISION@:@OP_LT_AGE@

bin_PROGRAMS = opustags opuspic2tag

opustags_SOURCES = src/clitags.c
opustags_LDADD = libopustags.la $(DEPS_LIBS)

opuspic2tag_SOURCES = src/clipic.c
opuspic2tag_LDADD = libopustags.la

dist_man_MANS = doc/opustags.1 doc/opuspic2tag.1

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = opustags.pc

debug:
	$(MAKE) CFLAGS="${CFLAGS} -O0 -ggdb -DOP_ENABLE_ASSERTIONS" all

EXTRA_DIST = \
 opustags.pc.in \
 LICENSE \
 doc/AUTHORS \
 doc/CHANGELOG \
 doc/sample_audiobook.opustags \
 README.md

libopustags: all
install-libopustags: install

# We check this every time make is run, with configure.ac being touched to
# trigger an update of the build system files if update_version changes the
# current PACKAGE_VERSION (or if package_version was modified manually by a
# user with either AUTO_UPDATE=no or no update_version script present - the
# latter being the normal case for tarball releases).
#
# We can't just add the package_version file to CONFIGURE_DEPENDENCIES since
# simply running autoconf will not actually regenerate configure for us when
# the content of that file changes (due to autoconf dependency checking not
# knowing about that without us creating yet another file for it to include).
#
# The MAKECMDGOALS check is a gnu-make'ism, but will degrade 'gracefully' for
# makes that don't support it.  The only loss of functionality is not forcing
# an update of package_version for `make dist` if AUTO_UPDATE=no, but that is
# unlikely to be a real problem for any real user.
$(top_srcdir)/configure.ac: force
	@case "$(MAKECMDGOALS)" in \
	    dist-hook)                             exit 0       ;; \
	    dist-* | dist | distcheck | distclean) _arg=release ;; \
	esac; \
	if ! $(top_srcdir)/update_version $$_arg 2> /dev/null; then \
	    if [ ! -e $(top_srcdir)/package_version ]; then \
		echo 'PACKAGE_VERSION="unknown"' > $(top_srcdir)/package_version; \
	    fi; \
	    . $(top_srcdir)/package_version || exit 1; \
	    [ "$(PACKAGE_VERSION)" != "$$PACKAGE_VERSION" ] || exit 0; \
	fi; \
	touch $@

force:

# Create a minimal package_version file when make dist is run.
dist-hook:
	echo 'PACKAGE_VERSION="$(PACKAGE_VERSION)"' > $(top_distdir)/package_version


.PHONY: libopustags opustags install-libopustags
