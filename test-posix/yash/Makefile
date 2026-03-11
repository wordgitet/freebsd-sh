.POSIX:
.SUFFIXES: .c .h .o .tst .trs

YASH ?= $(shell command -v yash 2>/dev/null)
TESTEE ?= ../../build/src/sh
TIMEOUT ?= 60s
TIMEOUT_CMD ?= timeout $(TIMEOUT)
CC ?= cc
CFLAGS ?= -O2
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?=

SOURCES = checkfg.c ptwrap.c resetsig.c
POSIX_SIGNAL_TEST_SOURCES = sigcont1-p.tst sigcont2-p.tst sigcont3-p.tst sigcont4-p.tst sigcont5-p.tst sigcont6-p.tst sigcont7-p.tst sigcont8-p.tst sighup1-p.tst sighup2-p.tst sighup3-p.tst sighup4-p.tst sighup5-p.tst sighup6-p.tst sighup7-p.tst sighup8-p.tst sigint1-p.tst sigint2-p.tst sigint3-p.tst sigint4-p.tst sigint5-p.tst sigint6-p.tst sigint7-p.tst sigint8-p.tst sigquit1-p.tst sigquit2-p.tst sigquit3-p.tst sigquit4-p.tst sigquit5-p.tst sigquit6-p.tst sigquit7-p.tst sigquit8-p.tst sigstop3-p.tst sigstop7-p.tst sigterm1-p.tst sigterm2-p.tst sigterm3-p.tst sigterm4-p.tst sigterm5-p.tst sigterm6-p.tst sigterm7-p.tst sigterm8-p.tst sigtstp3-p.tst sigtstp4-p.tst sigtstp7-p.tst sigtstp8-p.tst sigttin3-p.tst sigttin4-p.tst sigttin7-p.tst sigttin8-p.tst sigttou3-p.tst sigttou4-p.tst sigttou7-p.tst sigttou8-p.tst sigurg1-p.tst sigurg2-p.tst sigurg3-p.tst sigurg4-p.tst sigurg5-p.tst sigurg6-p.tst sigurg7-p.tst sigurg8-p.tst
POSIX_TEST_SOURCES = $(POSIX_SIGNAL_TEST_SOURCES) alias-p.tst andor-p.tst arith-p.tst async-p.tst bg-p.tst break-p.tst builtins-p.tst case-p.tst cd-p.tst cmdsub-p.tst command-p.tst comment-p.tst continue-p.tst declutil-p.tst dot-p.tst errexit-p.tst error-p.tst eval-p.tst exec-p.tst exit-p.tst export-p.tst fg-p.tst fnmatch-p.tst for-p.tst fsplit-p.tst function-p.tst getopts-p.tst grouping-p.tst if-p.tst input-p.tst job-p.tst kill1-p.tst kill2-p.tst kill3-p.tst kill4-p.tst lineno-p.tst nop-p.tst option-p.tst param-p.tst path-p.tst pipeline-p.tst ppid-p.tst quote-p.tst read-p.tst readonly-p.tst redir-p.tst return-p.tst set-p.tst shift-p.tst simple-p.tst startup-p.tst test-p.tst testtty-p.tst tilde-p.tst trap-p.tst umask-p.tst unset-p.tst until-p.tst wait-p.tst while-p.tst
POSIX_NOSIGNAL_EXCLUDES = $(POSIX_SIGNAL_TEST_SOURCES) bg-p.tst fg-p.tst job-p.tst kill1-p.tst kill2-p.tst kill3-p.tst kill4-p.tst trap-p.tst wait-p.tst
POSIX_NOSIGNAL_TEST_SOURCES = $(filter-out $(POSIX_NOSIGNAL_EXCLUDES),$(POSIX_TEST_SOURCES))
TEST_SOURCES ?= $(POSIX_TEST_SOURCES)
TEST_RESULTS = $(TEST_SOURCES:.tst=.trs)
RECHECK_LOGS = $(TEST_RESULTS)
TESTERS = $(SOURCES:.c=)
RUN_TEST = ./resetsig $(YASH) ./run-test.sh
SUMMARY = summary.log
BYPRODUCTS = $(TESTERS) $(TEST_RESULTS) $(SUMMARY) *.out *.err *.valgrind tmp.* *.dSYM

.PHONY: all test test-posix test-posix-nosignal clean clean-results check-yash check-testee tester

all: test-posix

check-yash:
	@test -n "$(YASH)" || { \
		echo "yash not found in PATH" >&2; \
		exit 1; \
	}

check-testee:
	@test -x "$(TESTEE)" || { \
		echo "testee not executable: $(TESTEE)" >&2; \
		exit 1; \
	}

test: check-yash check-testee
	rm -rf $(RECHECK_LOGS)
	@$(MAKE) $(SUMMARY)
	@tail -n 6 $(SUMMARY)
	@tail -n 6 $(SUMMARY) | grep '^ERROR:' | \
		(read _ fail_count && \
		if [ "$$fail_count" -ne 0 ]; then \
			echo See $(SUMMARY) for details; \
			exit 1; \
		fi)

test-posix:
	@$(MAKE) TEST_SOURCES='$(if $(TESTS),$(TESTS),$(POSIX_TEST_SOURCES))' test

test-posix-nosignal:
	@$(MAKE) TEST_SOURCES='$(if $(TESTS),$(TESTS),$(POSIX_NOSIGNAL_TEST_SOURCES))' test

$(SUMMARY): $(TEST_RESULTS)
	./summarize.sh $(TEST_RESULTS) >| $@

$(TEST_RESULTS): $(TESTERS)

.tst.trs:
	@set -- $(RUN_TEST) $(TESTEE) $<; \
	if grep -q %REQUIRETTY% $<; then \
		if ./ptwrap ./checkfg 2>/dev/null; then \
			set ./ptwrap "$$@"; \
		else \
			set ./enqueue.sh "$$@"; \
		fi; \
	fi; \
	echo "$$*"; \
	$(TIMEOUT_CMD) "$$@"

$(POSIX_SIGNAL_TEST_SOURCES:.tst=.trs): signal.sh

tester: $(TESTERS)

$(TESTERS):
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

clean-results:
	rm -f $(TEST_RESULTS) $(SUMMARY) *.out *.err *.valgrind tmp.*

clean: clean-results
	rm -f $(TESTERS)

.IGNORE: ptwrap
