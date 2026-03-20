TESTEE ?= ./build/src/sh
TIMEOUT ?= 160s

.PHONY: test-posix test-posix-nosignal test-posix-report test-posix-nosignal-report test-posix-freebsd test-posix-yash test-posix-smoosh test-primary test-guardrail clean-test-posix

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

test-posix-smoosh:
	$(MAKE) -C test-posix/smoosh TESTEE='$(abspath $(TESTEE))' \
		TEST_TIMEOUT='5' test-posix

test-primary: test-posix-report

test-guardrail: test-posix-freebsd

clean-test-posix:
	$(MAKE) -C test-posix/yash clean
	$(MAKE) -C test-posix/smoosh clean
