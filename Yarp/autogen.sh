#!/bin/sh

MAKE=`which gnumake 2>/dev/null`
if test ! -x "$MAKE" ; then MAKE=`which gmake 2>/dev/null` ; fi
if test ! -x "$MAKE" ; then MAKE=`which make 2>/dev/null` ; fi
HAVE_GNU_MAKE=`$MAKE --version|grep -c "Free Software Foundation"`

if test "$HAVE_GNU_MAKE" != "1"; then 
echo Only non-GNU make found: $MAKE
else
echo `$MAKE --version | head -1` found
fi

AUTORECONF=`which autoreconf 2>/dev/null`
if $AUTORECONF2.50 >/dev/null 2>&1
then AC_POSTFIX=2.50
elif $AUTORECONF --version >/dev/null 2>&1
then AC_POSTFIX=""
else
  echo 'you need autoreconf (2.58+ recommended) to generate the Makefile'
  exit 1
fi
echo `autoreconf$AC_POSTFIX --version | head -1` found

#AUTOCONF=`which autoconf`
#if $AUTOCONF2.50 >/dev/null 2>&1
#then AC_POSTFIX=2.50
#elif $AUTOCONF >/dev/null 2>&1
#then AC_POSTFIX=""
#else 
#  echo 'you need autoconf (2.58+ recommended) to generate the Makefile'
#  exit 1
#fi
#echo `autoconf$AC_POSTFIX --version | head -1` found

#AUTOMAKE=`which automake`
#if $AUTOMAKE-1.9 >/dev/null 2>&1
#then AM_POSTFIX=-1.9
#elif which automake-1.8 >/dev/null 2>&1
#then AM_POSTFIX=-1.8
#elif which automake-1.7 >/dev/null 2>&1
#then AM_POSTFIX=-1.7
#elif which automake-1.6 >/dev/null 2>&1
#then AM_POSTFIX=-1.6
#elif $$AUTOMAKE >/dev/null 2>&1
#then AM_POSTFIX=""
#else
#  echo 'you need automake (1.8.3+ recommended) to generate the Makefile'
#  exit 1
#fi
#echo `automake$AM_POSTFIX --version | head -1` found

AUTOPOINT=`which autopoint`
if ! test -x $AUTOPOINT; then
  echo 'you need autopoint (0.18+ recommended) to generate the Makefile'
  exit 1
fi
echo $($(basename $AUTOPOINT) --version | head -1) found

autoreconf$AC_POSTFIX --force --install --symlink --warnings=all
#autoconf$AC_POSTFIX
#autoheader$AC_POSTFIX
#aclocal$AM_POSTFIX
#automake$AM_POSTFIX --add-missing --force-missing --all
##gettextize
#autopoint
#if test ! -e po/Makevars
#then cp po/Makevars.template po/Makevars
#fi
#if test ! -e po/LINGUAS
#then touch po/LINGUAS
#fi
##libtoolize --force --copy
