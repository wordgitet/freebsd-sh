AC_INIT([neoash], [15.0-ver1])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config-compat.h])

dnl Checks for programs.
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

dnl Feature-test macros must be active on the compiler command line, not
dnl only in config-compat.h.  Our shadow headers use include_next, which
dnl pulls in system headers before config-compat.h is processed.  Without
dnl these macros on the command line, Bionic and other libc variants will
dnl not expose GNU extensions (strchrnul, etc.) when compiling those files.
dnl AC_USE_SYSTEM_EXTENSIONS defines them in config-compat.h and for
dnl configure checks, but may not add them to CPPFLAGS on non-GNU targets.
SHELL_EXT_CPPFLAGS="-D_GNU_SOURCE -D_ALL_SOURCE -D_DARWIN_C_SOURCE -D__EXTENSIONS__"
AC_SUBST([SHELL_EXT_CPPFLAGS])



AC_MSG_CHECKING([for build system compiler])
if test "$cross_compiling" = yes; then
	CC_FOR_BUILD=${CC_FOR_BUILD-cc}
	LDFLAGS_FOR_BUILD=${LDFLAGS_FOR_BUILD-}
	CFLAGS_FOR_BUILD=${CFLAGS_FOR_BUILD-}
else
	CC_FOR_BUILD=${CC}
	LDFLAGS_FOR_BUILD=${LDFLAGS}
	CFLAGS_FOR_BUILD=${CFLAGS}
fi
AC_MSG_RESULT(${CC_FOR_BUILD})
AC_SUBST(CC_FOR_BUILD)
AC_SUBST(LDFLAGS_FOR_BUILD)
AC_SUBST(CFLAGS_FOR_BUILD)

dnl Checks for header files.
AC_CHECK_HEADERS([sys/stat.h sys/types.h unistd.h string.h stdlib.h limits.h err.h paths.h])

dnl Checks for library functions.
AC_CHECK_FUNCS([strtoq strlcpy strlcat strtonum reallocf reallocarray errc warnc asprintf vasprintf getprogname wait3 eaccess faccessat funopen fopencookie])
AC_CHECK_DECLS([reallocarray], [], [], [[#include <stdlib.h>]])
AC_CHECK_DECLS([strchrnul], [], [], [[#include <string.h>]])

dnl Check for strchrnul respecting availability on macOS
AC_MSG_CHECKING([for working strchrnul])
strchrnul_save_CFLAGS="$CFLAGS"
# Add -Werror=unguarded-availability-new if compiler supports it (primarily macOS)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])], [
    # Check if compiler supports -Werror=unguarded-availability-new
    strchrnul_check_CFLAGS="$CFLAGS -Werror=unguarded-availability-new"
    CFLAGS_orig="$CFLAGS"
    CFLAGS="$strchrnul_check_CFLAGS"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])],
        [strchrnul_avail_flag="-Werror=unguarded-availability-new"],
        [strchrnul_avail_flag=""])
    CFLAGS="$CFLAGS_orig"
])
if test -n "$strchrnul_avail_flag"; then
    CFLAGS="$CFLAGS $strchrnul_avail_flag"
fi
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <string.h>]], [[return !strchrnul("", 0);]])],
    [ac_cv_func_strchrnul=yes],
    [ac_cv_func_strchrnul=no])
CFLAGS="$strchrnul_save_CFLAGS"
AC_MSG_RESULT([$ac_cv_func_strchrnul])
if test "$ac_cv_func_strchrnul" = yes; then
    AC_DEFINE([HAVE_STRCHRNUL], 1, [Define if you have a working strchrnul])
fi

dnl Check for libedit
AC_ARG_WITH([libedit],
    [AS_HELP_STRING([--with-libedit], [Compile with libedit support [default=check]])],
    [],
    [with_libedit=check])

use_libedit=no
if test "$with_libedit" != "no"; then
    AC_CHECK_LIB([edit], [el_init], [
        AC_CHECK_HEADER([histedit.h], [
            use_libedit=yes
            LIBS="$LIBS -ledit"
            AC_DEFINE([HAVE_LIBEDIT], 1, [Define if you have libedit])
        ], [
            if test "$with_libedit" = "yes"; then
                AC_MSG_ERROR([histedit.h header not found])
            fi
        ])
    ], [
        if test "$with_libedit" = "yes"; then
            AC_MSG_ERROR([libedit library not found])
        fi
    ])
fi

if test "$use_libedit" = "yes"; then
    dnl Check for fn_complete2 in libedit
    AC_CHECK_LIB([edit], [fn_complete2], [
        AC_DEFINE([HAVE_FN_COMPLETE2], 1, [Define if libedit has fn_complete2])
    ])
else
    AC_DEFINE([NO_HISTORY], 1, [Define to disable history if libedit is missing])
fi

dnl Check for warning flags.
dnl First probe whether the compiler understands -Werror=unknown-warning-option
dnl (a Clang feature).  If so, we use it as a strictness gate when testing
dnl subsequent -Wno-* flags: Clang silently accepts unknown -Wno-* flags at
dnl compile time but warns about them later, so the plain compile test is not
dnl sufficient.  GCC does not support the meta-flag, so it falls back to the
dnl plain test (GCC errors on truly unknown flags, which is strict enough).
AX_CFLAGS="-Wall -Wextra"
AC_MSG_CHECKING([if compiler supports -Werror=unknown-warning-option])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Werror=unknown-warning-option"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])],
    [flag_probe="-Werror=unknown-warning-option"; AC_MSG_RESULT([yes])],
    [flag_probe=""; AC_MSG_RESULT([no])])
CFLAGS="$save_CFLAGS"
for flag in -Wno-unknown-warning-option \
            -Wno-unused-result \
            -Wno-string-plus-int \
            -Wno-clobbered; do
    AC_MSG_CHECKING([if compiler supports $flag])
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS $flag_probe $flag"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])],
        [AX_CFLAGS="$AX_CFLAGS $flag"; AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])])
    CFLAGS="$save_CFLAGS"
done
AC_SUBST([AX_CFLAGS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
