/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2026 wordgitet.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void
usage(void)
{

	fputs("usage: pwd [-LP]\n", stderr);
	exit(1);
}

static char *
physical_pwd(void)
{
	char *p;
	size_t size;

	for (size = 128; ; size *= 2) {
		p = malloc(size);
		if (p == NULL)
			return NULL;
		if (getcwd(p, size) != NULL)
			return p;
		if (errno != ERANGE) {
			free(p);
			return NULL;
		}
		free(p);
		if (size > (size_t)-1 / 2)
			break;
	}
	errno = ERANGE;
	return NULL;
}

static char *
logical_pwd(void)
{
	const char *envpwd;
	struct stat pwdstat, dotstat;
	char *p;

	envpwd = getenv("PWD");
	if (envpwd == NULL || envpwd[0] != '/' ||
	    stat(envpwd, &pwdstat) < 0 || stat(".", &dotstat) < 0 ||
	    pwdstat.st_dev != dotstat.st_dev || pwdstat.st_ino != dotstat.st_ino)
		return physical_pwd();
	p = malloc(strlen(envpwd) + 1);
	if (p != NULL)
		strcpy(p, envpwd);
	return p;
}

int
main(int argc, char **argv)
{
	char *p;
	int ch, physical = 0;

	opterr = 0;
	while ((ch = getopt(argc, argv, "LP")) != -1) {
		switch (ch) {
		case 'L':
			physical = 0;
			break;
		case 'P':
			physical = 1;
			break;
		default:
			usage();
		}
	}
	if (optind != argc)
		usage();

	p = physical ? physical_pwd() : logical_pwd();
	if (p == NULL) {
		fprintf(stderr, "pwd: %s\n", strerror(errno));
		return 1;
	}
	if (puts(p) == EOF) {
		free(p);
		return 1;
	}
	free(p);
	return 0;
}
