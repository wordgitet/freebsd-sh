AM_CPPFLAGS = -DSHELL $(SHELL_EXT_CPPFLAGS) $(STANDARDS_CPPFLAGS) $(LIBEDIT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir) -I.
AM_CFLAGS = $(AX_CFLAGS)

bin_PROGRAMS = neoash

if BUILD_STANDALONE_UTILITIES
bin_PROGRAMS += neoash-echo neoash-kill neoash-printf neoash-test \
	neoash-pwd neoash-true neoash-false
endif

neoash_LDADD = $(LIBEDIT_LIBS)

neoash_SOURCES = \
	alias.c arith_yacc.c arith_yylex.c cd.c error.c eval.c exec.c expand.c \
	histedit.c input.c jobs.c mail.c main.c memalloc.c miscbltin.c \
	mystring.c options.c output.c parser.c redir.c show.c trap.c util.c var.c \
	bltin/echo.c bltin/kill.c bltin/printf.c bltin/test.c \
	compat/mktemp.c compat/setmode.c compat/signames.c compat/strlfuncs.c \
	alias.h arith.h arith_yacc.h cd.h error.h eval.h exec.h expand.h \
	input.h jobs.h mail.h main.h memalloc.h myhistedit.h mystring.h \
	options.h output.h parser.h redir.h shell.h show.h standards.h trap.h util.h var.h

nodist_neoash_SOURCES = \
	builtins.c builtins.h \
	nodes.c nodes.h \
	syntax.c syntax.h \
	token.h

if BUILD_STANDALONE_UTILITIES
# Standalone copies of selected builtins (no -DSHELL).  Prefixed names avoid
# shadowing system utilities when prepended to PATH.
UTILITY_BASE_CPPFLAGS = $(SHELL_EXT_CPPFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir) -I.
UTILITY_CPPFLAGS = $(UTILITY_BASE_CPPFLAGS) $(STANDARDS_CPPFLAGS)
# External echo follows XPG4/XSI semantics (-n is an operand, escapes on).
UTILITY_XSI_ECHO_CPPFLAGS = $(UTILITY_BASE_CPPFLAGS) \
	-DNEOASH_POSIX_VERSION=200809L -DNEOASH_XOPEN_VERSION=700

neoash_echo_SOURCES = bltin/echo.c
neoash_echo_CPPFLAGS = $(UTILITY_XSI_ECHO_CPPFLAGS)

neoash_kill_SOURCES = bltin/kill.c compat/signames.c
neoash_kill_CPPFLAGS = $(UTILITY_CPPFLAGS)

neoash_printf_SOURCES = bltin/printf.c compat/strlfuncs.c
neoash_printf_CPPFLAGS = $(UTILITY_CPPFLAGS)

neoash_test_SOURCES = bltin/test.c
neoash_test_CPPFLAGS = $(UTILITY_CPPFLAGS)
neoash_pwd_SOURCES = bltin/pwd.c
neoash_pwd_CPPFLAGS = $(UTILITY_CPPFLAGS)
neoash_true_SOURCES = bltin/true.c
neoash_true_CPPFLAGS = $(UTILITY_CPPFLAGS)
neoash_false_SOURCES = bltin/false.c
neoash_false_CPPFLAGS = $(UTILITY_CPPFLAGS)

install-exec-hook:
	for u in alias cd read unalias wait umask getopts; do \
		ln -sf neoash$(EXEEXT) '$(DESTDIR)$(bindir)/'$$u$(EXEEXT); \
	done
	for u in echo printf test kill pwd true false; do \
		ln -sf neoash-$$u$(EXEEXT) '$(DESTDIR)$(bindir)/'$$u$(EXEEXT); \
	done
	ln -sf neoash-test$(EXEEXT) '$(DESTDIR)$(bindir)/['$(EXEEXT)
endif

BUILT_SOURCES = token.h builtins.h builtins.c nodes.h nodes.c syntax.h syntax.c
CLEANFILES = $(BUILT_SOURCES) mknodes mksyntax

man_MANS = neoash.1

EXTRA_DIST = neoash.1 mktokens mkbuiltins builtins.def mknodes.c nodetypes nodes.c.pat mksyntax.c

token.h: mktokens
	$(AM_V_GEN)$(SHELL) $(srcdir)/mktokens .

builtins.h builtins.c: builtins.def mkbuiltins
	$(AM_V_GEN)$(SHELL) $(srcdir)/mkbuiltins $(srcdir) .

nodes.h nodes.c: mknodes nodetypes nodes.c.pat
	$(AM_V_GEN)./mknodes $(srcdir)/nodetypes $(srcdir)/nodes.c.pat nodes.h nodes.c

syntax.h syntax.c: mksyntax
	$(AM_V_GEN)./mksyntax syntax.h syntax.c

# Compile build tools for the build machine
COMPILE_FOR_BUILD = \
	$(CC_FOR_BUILD) $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) \
	$(AM_CFLAGS_FOR_BUILD) $(CFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD)

AM_CFLAGS_FOR_BUILD = -g -O2

mknodes: mknodes.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -o $@ $<

mksyntax: mksyntax.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -o $@ $<
