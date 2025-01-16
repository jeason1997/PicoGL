dnl Process this file with autoconf to produce a configure script.
AC_INIT([picogl], [0.1.0], [jserv@kaffe.org])
AC_CANONICAL_SYSTEM
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PREREQ([2.50])

dnl Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"

dnl -------------------------------------------------------------------------
AC_DEFUN([AC_CHECK_LIBRARY], [dnl
KSAVE_LIBS="$LIBS"
LIBS="$$3 $KSAVE_LIBS"
AC_CHECK_LIB($1,$2)
$3=`echo "$LIBS" | sed "s% $KSAVE_LIBS$%%"`
LIBS="$KSAVE_LIBS"
])

AC_ARG_WITH(
	[backend],
	AS_HELP_STRING(
		[--with-backend=GFX],
		[Pick up one of the graphic backends (Default: x11)]),
		[],
		[with_backend=x11]
)

case "$with_backend" in
	"x11") GFX_IMPL=x11
		;;
	"nano-x") GFX_IMPL=nanox
		;;
	"sdl") GFX_IMPL=sdl
		;;
        "vesafb") GFX_IMPL=vesafb
	        ;;
	*) echo "Invalid backend $with_backend!!" ; exit
		;;
esac
AC_SUBST(GFX_IMPL)
GLX_IMPL_HEADER="\$(top_srcdir)/backends/\$(GFX_IMPL)/glx_impl.h"
AC_SUBST(GLX_IMPL_HEADER)
AM_CONDITIONAL(COND_SDL, test x"$GFX_IMPL" = x"sdl")

if test x"$with_backend" = x"x11"; then
	dnl Checks for libraries for X related libraries.
	AC_PATH_XTRA
	OLIBS=$LIBS
	LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $OLIBS"
        AC_CHECK_LIBRARY(Xext, XShmQueryExtension, X_PRE_LIBS)
        AC_CHECK_HEADERS([X11/extensions/XShm.h], [], [],
        [[#include <X11/Xlib.h>
        ]])
	LIBS=$OLIBS

	dnl For each -L switch in X_LIBS, add a corresponding -R, unless
	dnl AC_PATH_XTRA has already taken care of them.
	case "$X_LIBS" in
		"-R"* | *" -R"*)
			;;
		"-L"* | *" -L"*)
			for ac_xlibdir in $X_LIBS; do
				case "$ac_xlibdir" in
					-L*)
						X_LIBS="$X_LIBS "`echo "$ac_xlibdir" | sed 's/^-L/-R/'`
						;;
				esac
			done
			;;
	esac

        GFX_LIBS="$X_LIBS $X_PRE_LIBS -lX11 -lXext"
        AC_SUBST(GFX_LIBS)
fi

if test x"$with_backend" = x"nano-x"; then
	AC_ARG_WITH(nanox-include,
		AS_HELP_STRING([--with-nanox-include=dir],
		[Specify Nano-X include directory]),
		[ NANOXINC=$withval ],
		[ NANOXINC="/usr/include/microwin" ])

	AC_ARG_WITH(nanox-library,
		AS_HELP_STRING([--with-nanox-library=dir],
		[Specify NanoX library directory]),
		[ NANOXLIB=$withval ],
		[ NANOXLIB="/usr/lib"])

	NANOX_CFLAGS="-I${NANOXINC}"
	GFX_LIBS="-L${NANOXLIB} -lnano-X -lmwengine -lmwdrivers -lmwfonts"
        AC_SUBST(NANOX_CFLAGS)
        AC_SUBST(GFX_LIBS)
fi

if test x"$with_backend" = x"sdl"; then
	AM_PATH_SDL(1.2.0, :, AC_MSG_ERROR([*** libSDL not found: http://www.libsdl.org]))
	GFX_LIBS="$SDL_LIBS"        
        AC_SUBST(GFX_LIBS)
fi

AC_ARG_ENABLE(
	[fixed-point],
	AS_HELP_STRING(
		[--enable-fixed-point],
		[Use fixed-pointing. (Default: no)]),
	[],
	[enable_fixed_point="no"])

FIXED_POINT_CFLAGS="-DUSE_FIXED_POINT \
	-msoft-float -ffast-math \
	-nostdlib -fno-common -ffreestanding \
	-fno-builtin -fomit-frame-pointer -fstrict-aliasing"

case "$enable_fixed_point" in
	yes) CPPFLAGS="$CPPFLAGS $FIXED_POINT_CFLAGS"
		;;
esac
AC_MSG_CHECKING([whether to enable fixed-pointing machinery])
AC_MSG_RESULT($enable_fixed_point)

AC_OUTPUT([
	Makefile
	src/Makefile
	src/glu/Makefile
	examples/Makefile
])

echo
echo $PACKAGE $VERSION

echo PicoGL graphic backend........ : $with_backend
echo
echo PicoGL uses fixed-pointing.... : $enable_fixed_point
echo
echo configure complete, now type \'make\'
echo
