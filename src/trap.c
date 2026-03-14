/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "shell.h"
#include "main.h"
#include "nodes.h"	/* for other headers */
#include "eval.h"
#include "jobs.h"
#include "show.h"
#include "options.h"
#include "syntax.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "trap.h"
#include "mystring.h"
#include "builtins.h"
#ifndef NO_HISTORY
#include "myhistedit.h"
#endif


/*
 * Sigmode records the current value of the signal handlers for the various
 * modes.  A value of zero means that the current handler is not known.
 * S_HARD_IGN indicates that the signal was ignored on entry to the shell,
 */

#define S_DFL 1			/* default signal handling (SIG_DFL) */
#define S_CATCH 2		/* signal is caught */
#define S_IGN 3			/* signal is ignored (SIG_IGN) */
#define S_HARD_IGN 4		/* signal is ignored permanently */
#define S_RESET 5		/* temporary - to reset a hard ignored sig */


static char sigmode[NSIG];	/* current value of signal */
static char siginherited[NSIG];	/* signal was ignored on shell entry */
static int startupiflag;	/* shell was invoked with -i */
static int trap_force_signo;	/* trap is explicitly changing this signal */
static int trap_default_signo;	/* trap is explicitly restoring true default */
static int trap_saved_status;	/* status before the current trap action */
static int trap_saved_active;	/* non-zero while running a trap action */
volatile sig_atomic_t pendingsig;	/* indicates some signal received */
volatile sig_atomic_t pendingsig_waitcmd;	/* indicates wait builtin should be interrupted */
volatile sig_atomic_t gotsigchld;	/* indicates a child status change was delivered */
static int in_dotrap;			/* do we execute in a trap handler? */
static char *volatile trap[NSIG];	/* trap handler commands */
static volatile sig_atomic_t gotsig[NSIG];
				/* indicates specified signal received */
static int ignore_sigchld;	/* Used while handling SIGCHLD traps. */
static int last_trapsig;

static int exiting;		/* exitshell() has been called */
static int exiting_exitstatus;	/* value passed to exitshell() */

static int getsigaction(int, sig_t *);
static int exitsig(int);
static int
is_tty_stopsig(int signo)
{

	return (signo == SIGTSTP || signo == SIGTTIN || signo == SIGTTOU);
}

static int
tty_stopsig_preserve_ign(int signo)
{
	if (!siginherited[signo] || !is_tty_stopsig(signo))
		return 0;
	if (rootshell && mflag)
		return !startupiflag;
	if (iflag)
		return 0;
	return 1;
}

static int
tty_stopsig_keep_shell_default(int signo)
{

	return (startupiflag && rootshell && mflag && is_tty_stopsig(signo));
}


/*
 * Map a string to a signal number.
 *
 * Note: the signal number may exceed NSIG.
 */
static int
sigstring_to_signum(char *sig)
{

	if (is_number(sig)) {
		int signo;

		signo = atoi(sig);
		return ((signo >= 0 && signo < NSIG) ? signo : (-1));
	} else if (strcasecmp(sig, "EXIT") == 0) {
		return (0);
	} else {
		int n;

		if (strncasecmp(sig, "SIG", 3) == 0)
			sig += 3;
		for (n = 1; n < NSIG; n++) {
			const char *sign = signum_to_signame(n);
			if (sign && strcasecmp(sign, sig) == 0)
				return (n);
		}
	}
	return (-1);
}


/*
 * Print a list of valid signal names.
 */
static void
printsignals(void)
{
	int n, outlen;

	outlen = 0;
	for (n = 1; n < NSIG; n++) {
		const char *sign = signum_to_signame(n);
		if (sign) {
			out1fmt("%s", sign);
			outlen += strlen(sign);
		} else {
			out1fmt("%d", n);
			outlen += 3;	/* good enough */
		}
		++outlen;
		if (outlen > 71 || n == NSIG - 1) {
			out1str("\n");
			outlen = 0;
		} else {
			out1c(' ');
		}
	}
}


/*
 * The trap builtin.
 */
static void
printtrap(int signo, int show_default)
{
	const char *sign;

	if (!show_default && trap[signo] == NULL)
		return;

	sign = signum_to_signame(signo);
	out1str("trap -- ");
	if (trap[signo] == NULL) {
		if (show_default)
			out1fmt("%d ", signo);
		else
			out1str("- ");
	} else
		out1qstr(trap[signo]);
	if (signo == 0) {
		out1str(" EXIT\n");
	} else if (sign) {
		out1fmt(" %s\n", sign);
	} else {
		out1fmt(" %d\n", signo);
	}
}

int
trapcmd(int argc __unused, char **argv)
{
	char *action;
	int signo;
	int errors = 0;
	int i;
	int print_only = 0;
	int show_default = 0;
	int force_default = 0;

	while ((i = nextopt("lp")) != '\0') {
		switch (i) {
		case 'l':
			printsignals();
			return (0);
		case 'p':
			print_only = 1;
			show_default = 1;
			break;
		}
	}
	argv = argptr;

	if (print_only) {
		if (*argv == NULL) {
			for (signo = 0 ; signo < NSIG ; signo++)
				printtrap(signo, show_default);
			return 0;
		}
		for (; *argv; argv++) {
			if ((signo = sigstring_to_signum(*argv)) == -1) {
				warning("bad signal %s", *argv);
				errors = 1;
				continue;
			}
			printtrap(signo, show_default);
		}
		return errors;
	}

	if (*argv == NULL) {
		for (signo = 0 ; signo < NSIG ; signo++) {
			printtrap(signo, 0);
		}
		return 0;
	}
	action = NULL;
	if (*argv && !is_number(*argv)) {
		if (strcmp(*argv, "-") == 0) {
			force_default = 1;
			argv++;
		} else {
			action = *argv;
			argv++;
		}
	} else if (*argv) {
		force_default = 1;
	}
	for (; *argv; argv++) {
		if ((signo = sigstring_to_signum(*argv)) == -1) {
			warning("bad signal %s", *argv);
			errors = 1;
			continue;
		}
		INTOFF;
		if (action)
			action = savestr(action);
		if (trap[signo])
			ckfree(trap[signo]);
		trap[signo] = action;
		if (signo != 0) {
			trap_force_signo = signo;
			if (force_default && is_tty_stopsig(signo) && iflag)
				trap_default_signo = signo;
			setsignal(signo);
			trap_default_signo = 0;
			trap_force_signo = 0;
		}
		INTON;
	}
	return errors;
}


/*
 * Clear traps on a fork.
 */
void
clear_traps(void)
{
	char *volatile *tp;

	for (tp = trap ; tp <= &trap[NSIG - 1] ; tp++) {
		if (*tp && **tp) {	/* trap not NULL or SIG_IGN */
			INTOFF;
			ckfree(*tp);
			*tp = NULL;
			if (tp != &trap[0])
				setsignal(tp - trap);
			INTON;
		}
	}
}


/*
 * Check if we have any traps enabled.
 */
int
have_traps(void)
{
	char *volatile *tp;

	for (tp = trap ; tp <= &trap[NSIG - 1] ; tp++) {
		if (*tp && **tp)	/* trap not NULL or SIG_IGN */
			return 1;
	}
	return 0;
}

/*
 * Set the signal handler for the specified signal.  The routine figures
 * out what it should be set to.
 */
void
setsignal(int signo)
{
	int action;
	sig_t sigact = SIG_DFL;
	struct sigaction sa;
	char *t;

	if ((t = trap[signo]) == NULL)
		action = S_DFL;
	else if (*t != '\0')
		action = S_CATCH;
	else
		action = S_IGN;
	if (((tty_stopsig_preserve_ign(signo) ||
	    tty_stopsig_keep_shell_default(signo)) &&
	    trap_default_signo != signo))
		action = S_IGN;
	if (action == S_DFL) {
		if (trap_default_signo != signo) {
			switch (signo) {
			case SIGINT:
				action = S_CATCH;
				break;
			case SIGQUIT:
#ifdef DEBUG
				if (debug)
					break;
#endif
				if (rootshell && iflag)
					action = S_IGN;
				break;
			case SIGTERM:
				if (rootshell && iflag)
					action = S_IGN;
				break;
#if JOBS
			case SIGTSTP:
			case SIGTTIN:
			case SIGTTOU:
				if (rootshell && mflag)
					action = S_IGN;
				break;
#endif
			}
		}
	}
	if (signo == SIGCHLD)
		action = S_CATCH;

	t = &sigmode[signo];
	if (*t == 0) {
		/*
		 * current setting unknown
		 */
		if (!getsigaction(signo, &sigact)) {
			/*
			 * Pretend it worked; maybe we should give a warning
			 * here, but other shells don't. We don't alter
			 * sigmode, so that we retry every time.
			 */
			return;
		}
		if (sigact == SIG_IGN) {
			siginherited[signo] = 1;
			if (mflag && (signo == SIGTSTP ||
			     signo == SIGTTIN || signo == SIGTTOU)) {
				*t = S_IGN;	/* don't hard ignore these */
			} else
				*t = S_HARD_IGN;
		} else {
			*t = S_RESET;	/* force to be set */
		}
	}
	if (trap_default_signo == signo &&
	    (*t == S_HARD_IGN || *t == S_IGN))
		*t = S_RESET;
	if (*t == S_HARD_IGN && trap_force_signo == signo && iflag &&
	    !tty_stopsig_preserve_ign(signo))
		*t = S_RESET;
	if (*t == S_HARD_IGN || *t == action)
		return;
	switch (action) {
		case S_DFL:	sigact = SIG_DFL;	break;
		case S_CATCH:  	sigact = onsig;		break;
		case S_IGN:	sigact = SIG_IGN;	break;
	}
	*t = action;
	sa.sa_handler = sigact;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signo, &sa, NULL);
}


/*
 * Return the current setting for sig w/o changing it.
 */
static int
getsigaction(int signo, sig_t *sigact)
{
	struct sigaction sa;

	if (sigaction(signo, (struct sigaction *)0, &sa) == -1)
		return 0;
	*sigact = (sig_t) sa.sa_handler;
	return 1;
}


/*
 * Ignore a signal.
 */
void
ignoresig(int signo)
{

	if (sigmode[signo] == 0)
		setsignal(signo);
	if (sigmode[signo] != S_IGN && sigmode[signo] != S_HARD_IGN) {
		signal(signo, SIG_IGN);
		sigmode[signo] = S_IGN;
	}
}


int
issigtrapped(int signo)
{

	return (trap[signo] != NULL && *trap[signo] != '\0');
}


int
is_inherited_sig_ign(int signo)
{

	return (siginherited[signo] != 0);
}


int
issigchldtrapped(void)
{

	return issigtrapped(SIGCHLD);
}


/*
 * Signal handler.
 */
void
onsig(int signo)
{

	if (signo == SIGCHLD)
		gotsigchld = 1;

	if (signo == SIGINT && trap[SIGINT] == NULL) {
		if (suppressint)
			SET_PENDING_INT;
		else
			onint();
		return;
	}

	/* If we are currently in a wait builtin, prepare to break it */
	if (signo == SIGINT || signo == SIGQUIT)
		pendingsig_waitcmd = signo;

	if (trap[signo] != NULL && trap[signo][0] != '\0' &&
	    (signo != SIGCHLD || !ignore_sigchld)) {
		gotsig[signo] = 1;
		pendingsig = signo;
		pendingsig_waitcmd = signo;
	}
}


/*
 * Called to execute a trap.  Perhaps we should avoid entering new trap
 * handlers while we are executing a trap handler.
 */
void
dotrap(void)
{
	struct stackmark smark;
	int i;
	int inton;
	int savestatus, prev_evalskip, prev_skipcount;
	int prev_trap_saved_status, prev_trap_saved_active, prev_trapsig;

	in_dotrap++;
	for (;;) {
		pendingsig = 0;
		pendingsig_waitcmd = 0;
		for (i = 1; i < NSIG; i++) {
			if (gotsig[i]) {
				gotsig[i] = 0;
				if (trap[i]) {
					/*
					 * Ignore SIGCHLD to avoid infinite
					 * recursion if the trap action does
					 * a fork.
					 */
					if (i == SIGCHLD)
						ignore_sigchld++;

					/*
					 * Backup current evalskip
					 * state and reset it before
					 * executing a trap, so that the
					 * trap is not disturbed by an
					 * ongoing break/continue/return
					 * statement.
					 */
					prev_evalskip  = evalskip;
					prev_skipcount = skipcount;
					prev_trap_saved_status = trap_saved_status;
					prev_trap_saved_active = trap_saved_active;
					prev_trapsig = last_trapsig;
					evalskip = 0;

					last_trapsig = i;
					savestatus = exitstatus;
					trap_saved_active = 1;
					trap_saved_status = savestatus;
					setstackmark(&smark);
					inton = is_int_on();
					FORCEINTON;
					evalstring(stsavestr(trap[i]), 0);
					SETINTON(inton);
					popstackmark(&smark);
					trap_saved_status = prev_trap_saved_status;
					trap_saved_active = prev_trap_saved_active;
					last_trapsig = prev_trapsig;

					/*
					 * If such a command was not
					 * already in progress, allow a
					 * break/continue/return in the
					 * trap action to have an effect
					 * outside of it.
					 */
					if (evalskip == 0 ||
					    prev_evalskip != 0) {
						evalskip  = prev_evalskip;
						skipcount = prev_skipcount;
						exitstatus = savestatus;
					}

					if (i == SIGCHLD)
						ignore_sigchld--;
				}
				break;
			}
		}
		if (i >= NSIG)
			break;
	}
	in_dotrap--;
}


void
trap_init(void)
{
	startupiflag = iflag;
	setsignal(SIGCHLD);
	setsignal(SIGINT);
	setsignal(SIGQUIT);
}


/*
 * Controls whether the shell is interactive or not based on iflag.
 */
void
setinteractive(void)
{
	if (rootshell)
		setsignal(SIGQUIT);
	setsignal(SIGTERM);
}

int
gettrapstatus(void)
{
	return trap_saved_active && rootshell ? trap_saved_status : -1;
}


/*
 * Called to exit the shell.
 */
void
exitshell(int status)
{
	TRACE(("exitshell(%d) pid=%d\n", status, getpid()));
	exiting = 1;
	exiting_exitstatus = status;
	exitshell_savedstatus();
}

void
exitshell_savedstatus(void)
{
	struct jmploc loc1, loc2;
	char *p;
	int sig = 0;
	sigset_t sigs;

	if (!exiting) {
		if (trap_saved_active && rootshell) {
			if (last_trapsig != 0 && last_trapsig != SIGINT)
				exiting_exitstatus = 384 + last_trapsig;
			else
				exiting_exitstatus = trap_saved_status;
		}
		else
			exiting_exitstatus = oexitstatus;
	}
	exitstatus = oexitstatus = exiting_exitstatus;
	if (!setjmp(loc1.loc)) {
		handler = &loc1;
		if ((p = trap[0]) != NULL && *p != '\0') {
			int prev_trap_saved_status, prev_trap_saved_active;
			int prev_trapsig;

			/*
			 * Reset evalskip, or the trap on EXIT could be
			 * interrupted if the last command was a "return".
			 */
			evalskip = 0;
			trap[0] = NULL;
			prev_trap_saved_status = trap_saved_status;
			prev_trap_saved_active = trap_saved_active;
			prev_trapsig = last_trapsig;
			trap_saved_active = 1;
			trap_saved_status = exiting_exitstatus;
			last_trapsig = 0;
			FORCEINTON;
			evalstring(p, 0);
			trap_saved_status = prev_trap_saved_status;
			trap_saved_active = prev_trap_saved_active;
			last_trapsig = prev_trapsig;
		}
	} else if (exception != EXEXIT) {
		exiting_exitstatus = exitstatus;
	}
	if (!setjmp(loc2.loc)) {
		handler = &loc2;		/* probably unnecessary */
		FORCEINTON;
		flushall();
#if JOBS
		setjobctl(0);
#endif
#ifndef NO_HISTORY
		histsave();
#endif
	}
	sig = exitsig(exiting_exitstatus);
	if (sig != 0 && sig != SIGSTOP && sig != SIGTSTP && sig != SIGTTIN &&
	    sig != SIGTTOU) {
		signal(sig, SIG_DFL);
		sigemptyset(&sigs);
		sigaddset(&sigs, sig);
		sigprocmask(SIG_UNBLOCK, &sigs, NULL);
		kill(getpid(), sig);
		/* If the default action is to ignore, fall back to _exit(). */
	}
	_exit(exiting_exitstatus);
}
static int
exitsig(int status)
{
	if (status >= 384 && status < 384 + NSIG)
		return status - 384;
	if (status >= 128 && status < 128 + NSIG)
		return status - 128;
	return 0;
}
