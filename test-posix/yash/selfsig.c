/* selfsig.c: terminate this process with the requested signal */

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	struct sigaction action;
	char *end;
	long signo;

	if (argc != 2)
		return 2;
	errno = 0;
	signo = strtol(argv[1], &end, 10);
	if (errno != 0 || *argv[1] == '\0' || *end != '\0' ||
	    signo <= 0 || signo > INT_MAX)
		return 2;
	if (signo != SIGKILL && signo != SIGSTOP) {
		action.sa_handler = SIG_DFL;
		action.sa_flags = 0;
		sigemptyset(&action.sa_mask);
		if (sigaction((int)signo, &action, NULL) != 0)
			return 2;
	}
	if (kill(getpid(), (int)signo) != 0)
		return 2;
	return 127;
}
