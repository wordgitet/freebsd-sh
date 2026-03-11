TESTEE ?= ./build/src/sh
TIMEOUT ?= 60s

.PHONY: test-posix test-posix-nosignal test-posix-report test-posix-nosignal-report test-posix-freebsd test-posix-yash test-primary test-guardrail clean-test-posix

test-posix: test-posix-yash

test-posix-nosignal:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix-nosignal

test-posix-report:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix-report

test-posix-nosignal-report:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix-nosignal-report

test-posix-freebsd:
	$(MAKE) -C test-posix/freebsd -f Makefile.local TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' TESTS='$(TESTS)' test-posix-freebsd

test-posix-yash:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix

test-primary: test-posix-report

test-guardrail: test-posix-freebsd

clean-test-posix:
	$(MAKE) -C test-posix/yash clean
