# autoconf source script for generating configure

dnl The package_version file will be automatically synced to the git revision
dnl by the update_version script when configured in the repository, but will
dnl remain constant in tarball releases unless it is manually edited.
m4_define([CURRENT_VERSION],
          m4_esyscmd([ ./update_version 2>/dev/null || true
                       if test -e package_version; then
                           . ./package_version
                           printf "$PACKAGE_VERSION"
                       else
                           printf "unknown"
                       fi ]))

AC_INIT([opustags],[CURRENT_VERSION],[zvezdochiot@github.com],[opustags],[https://github.com/zvezdochiot/opustags])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

AM_INIT_AUTOMAKE([1.11 foreign no-define dist-zip subdir-objects])
AM_MAINTAINER_MODE([enable])

AC_C_INLINE

LT_INIT

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_DEFINE([OPE_BUILD], [], [This is a build of libopustags])

dnl Library versioning for libtool.
dnl Please update these for releases.
dnl CURRENT, REVISION, AGE
dnl - library source changed -> increment REVISION
dnl - interfaces added/removed/changed -> increment CURRENT, REVISION = 0
dnl - interfaces added -> increment AGE
dnl - interfaces removed -> AGE = 0

OP_LT_CURRENT=4
OP_LT_REVISION=0
OP_LT_AGE=3

AC_SUBST(OP_LT_CURRENT)
AC_SUBST(OP_LT_REVISION)
AC_SUBST(OP_LT_AGE)

CC_CHECK_CFLAGS_APPEND(
  [-Wall])

# Platform-specific tweaks
case $host in
  *-mingw*)
    # -std=c89 causes some warnings under mingw.
    CC_CHECK_CFLAGS_APPEND([-U__STRICT_ANSI__])
    # We need WINNT>=0x501 (WindowsXP) for getaddrinfo/freeaddrinfo.
    # It's okay to define this even when HTTP support is disabled, as it only
    #  affects header declarations, not linking (unless we actually use some
    #  XP-only functions).
    AC_DEFINE_UNQUOTED(_WIN32_WINNT,0x501,
     [We need at least WindowsXP for getaddrinfo/freeaddrinfo])
    host_mingw=true
    ;;
esac
AM_CONDITIONAL(OP_WIN32, test "$host_mingw" = "true")

m4_ifndef([PKG_PROG_PKG_CONFIG],
  [m4_fatal([Could not locate the pkg-config autoconf macros.
Please make sure pkg-config is installed and, if necessary, set the environment
variable ACLOCAL="aclocal -I/path/to/pkg.m4".])])

PKG_CHECK_MODULES([DEPS], [ogg >= 1.3])

AC_CONFIG_FILES([
  Makefile
  opustags.pc
])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT

AC_MSG_NOTICE([
------------------------------------------------------------------------
  $PACKAGE_NAME $PACKAGE_VERSION: Automatic configuration OK.

------------------------------------------------------------------------
])
