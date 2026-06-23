AM_CPPFLAGS = -DSHELL -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir) -I.
AM_CFLAGS = $(AX_CFLAGS)

bin_PROGRAMS = sh

sh_SOURCES = \
	alias.c arith_yacc.c arith_yylex.c cd.c error.c eval.c exec.c expand.c \
	histedit.c input.c jobs.c mail.c main.c memalloc.c miscbltin.c \
	mystring.c options.c output.c parser.c redir.c show.c trap.c var.c \
	bltin/echo.c bltin/kill.c bltin/printf.c bltin/test.c \
	compat/mktemp.c compat/setmode.c compat/signames.c compat/strlfuncs.c \
	alias.h arith.h arith_yacc.h cd.h error.h eval.h exec.h expand.h \
	input.h jobs.h mail.h main.h memalloc.h myhistedit.h mystring.h \
	options.h output.h parser.h redir.h shell.h show.h trap.h var.h

nodist_sh_SOURCES = \
	builtins.c builtins.h \
	nodes.c nodes.h \
	syntax.c syntax.h \
	token.h

BUILT_SOURCES = token.h builtins.h builtins.c nodes.h nodes.c syntax.h syntax.c
CLEANFILES = $(BUILT_SOURCES) mknodes mksyntax

man_MANS = sh.1

EXTRA_DIST = sh.1 mktokens mkbuiltins builtins.def mknodes.c nodetypes nodes.c.pat mksyntax.c

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
	$(AM_CFLAGS_FOR_BUILD) $(CFLAGS_FOR_BUILD)

AM_CFLAGS_FOR_BUILD = -g -O2

mknodes: mknodes.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -o $@ $<

mksyntax: mksyntax.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -o $@ $<
