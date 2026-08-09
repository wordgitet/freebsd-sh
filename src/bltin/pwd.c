/*-
 * SPDX-License-Identifier: BSD-3-Clause
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
