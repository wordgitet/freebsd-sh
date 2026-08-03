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

#include <sys/cdefs.h>
/*
 * Echo command.
 */

#ifdef SHELL
#define main echocmd
#include "bltin.h"
#else
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#endif

#include "../standards.h"

/* #define eflag 1 */

#ifdef SHELL
static int
echo_putchar(int c)
{
	putchar(c);
	return 0;
}
#else
static int
echo_putchar(int c)
{
	if (putchar(c) == EOF && errno == EPIPE) {
		fprintf(stderr, "echo: write error: Broken pipe\n");
		return 1;
	}
	return 0;
}
#endif

int
main(int argc, char *argv[])
{
	char **ap;
	char *p;
	char c;
	int count;
	int nflag = 0;
#if NEOASH_XSI
	int eflag = 1;
#else
	int eflag = 0;
#endif

	ap = argv;
	if (argc)
		ap++;
#ifndef SHELL
	(void)signal(SIGPIPE, SIG_IGN);
#endif
#if NEOASH_EXTENSIONS
	if ((p = *ap) != NULL) {
		if (strcmp(p, "-n") == 0) {
			nflag++;
			ap++;
		} else if (strcmp(p, "-e") == 0) {
			eflag++;
			ap++;
		}
	}
#endif
	while ((p = *ap++) != NULL) {
		while ((c = *p++) != '\0') {
			if (c == '\\' && eflag) {
				switch (*p++) {
				case 'a':  c = '\a';  break;
				case 'b':  c = '\b';  break;
				case 'c':  return 0;		/* exit */
				case 'e':  c = '\033';  break;
				case 'f':  c = '\f';  break;
				case 'n':  c = '\n';  break;
				case 'r':  c = '\r';  break;
				case 't':  c = '\t';  break;
				case 'v':  c = '\v';  break;
				case '\\':  break;		/* c = '\\' */
				case '0':
					c = 0;
					count = 3;
					while (--count >= 0 && (unsigned)(*p - '0') < 8)
						c = (c << 3) + (*p++ - '0');
					break;
				default:
					p--;
					break;
				}
			}
			if (echo_putchar(c))
				return 1;
		}
		if (*ap && echo_putchar(' '))
			return 1;
	}
	if (!nflag && echo_putchar('\n'))
		return 1;
#ifndef SHELL
	if (fflush(stdout) != 0 && errno == EPIPE) {
		fprintf(stderr, "echo: write error: Broken pipe\n");
		return 1;
	}
#endif
	return 0;
}
