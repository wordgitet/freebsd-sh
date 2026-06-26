AC_INIT([neoash], [15.0-ver1])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config-compat.h])

dnl Checks for programs.
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE



AC_MSG_CHECKING([for build system compiler])
if test "$cross_compiling" = yes; then
	CC_FOR_BUILD=${CC_FOR_BUILD-cc}
else
	CC_FOR_BUILD=${CC}
fi
AC_MSG_RESULT(${CC_FOR_BUILD})
AC_SUBST(CC_FOR_BUILD)

dnl Checks for header files.
AC_CHECK_HEADERS([sys/stat.h sys/types.h unistd.h string.h stdlib.h limits.h err.h paths.h])

dnl Checks for library functions.
AC_CHECK_FUNCS([strtoq strlcpy strlcat strtonum reallocf reallocarray errc warnc asprintf vasprintf getprogname wait3 eaccess faccessat])

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

dnl Check for warning flags
AX_CFLAGS="-Wall -Wextra"
for flag in -Wno-unused-result -Wno-string-plus-int -Wno-clobbered; do
    AC_MSG_CHECKING([if compiler supports $flag])
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS $flag"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])], [
        AX_CFLAGS="$AX_CFLAGS $flag"
        AC_MSG_RESULT([yes])
    ], [
        AC_MSG_RESULT([no])
    ])
    CFLAGS="$save_CFLAGS"
done
AC_SUBST([AX_CFLAGS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
