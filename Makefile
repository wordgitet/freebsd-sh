TESTEE ?= ./build/src/sh
TIMEOUT ?= 60s

.PHONY: test-posix test-posix-nosignal test-posix-yash clean-test-posix

test-posix: test-posix-yash

test-posix-nosignal:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix-nosignal

test-posix-yash:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' \
		TIMEOUT='$(TIMEOUT)' test-posix

clean-test-posix:
	$(MAKE) -C test-posix/yash clean
