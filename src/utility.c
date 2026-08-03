/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Run a single builtin as a standalone POSIX utility when argv[0] names one.
 * Same pattern as ksh93 -p wrappers: the shell binary is also on PATH as cd,
 * pwd, read, and so on.
 */

#include <string.h>

#include "shell.h"
#include "options.h"
#include "exec.h"
#include "builtins.h"
#include "error.h"
#include "output.h"
#include "trap.h"
#include "var.h"
#include "cd.h"
#include "eval.h"
#include "main.h"
#include "memalloc.h"

static const char *const standalone_utilities[] = {
	"cd",
	"chdir",
	"pwd",
	"read",
	"wait",
	"umask",
	"true",
	"false",
	"getopts",
	":",
	NULL
};

const char *
standalone_utility_name(const char *argv0)
{
	const char *name;
	size_t i;

	if (argv0 == NULL || *argv0 == '\0')
		return (NULL);
	name = strrchr(argv0, '/');
	name = name != NULL ? name + 1 : argv0;
	if (strcmp(name, "neoash") == 0 || strcmp(name, "sh") == 0)
		return (NULL);
	for (i = 0; standalone_utilities[i] != NULL; i++)
		if (strcmp(name, standalone_utilities[i]) == 0)
			return (standalone_utilities[i]);
	return (NULL);
}

int
standalone_utility_main(int argc, char **argv, const char *name)
{
	struct jmploc jmploc;
	struct jmploc *savehandler;
	struct stackmark smark;
	int special;
	int idx;
	int status;

	handler = &main_handler;
	if (setjmp(main_handler.loc))
		return (exitstatus);

	rootpid = getpid();
	rootshell = 1;
	INTOFF;
	initvar();
	setstackmark(&smark);
	trap_init();
	pwd_init(0);
	INTON;

	idx = find_builtin(name, &special);
	if (idx < 0)
		return (127);

	savehandler = handler;
	if (setjmp(jmploc.loc)) {
		handler = savehandler;
		status = exitstatus;
		goto out;
	}
	handler = &jmploc;

	commandname = argv[0];
	argptr = argv + 1;
	nextopt_optptr = NULL;
	outclearerror(out1);

	status = (*builtinfunc[idx])(argc, argv);
	flushall();
	if (outiserror(out1)) {
		if (status == 0 || status == 1)
			status = 2;
	}
	handler = savehandler;

out:
	popstackmark(&smark);
	return (status);
}
