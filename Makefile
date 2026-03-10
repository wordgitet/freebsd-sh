TESTEE ?= ./build/src/sh

.PHONY: test-posix test-posix-yash clean-test-posix

test-posix: test-posix-yash

test-posix-yash:
	$(MAKE) -C test-posix/yash TESTEE='$(abspath $(TESTEE))' test-posix

clean-test-posix:
	$(MAKE) -C test-posix/yash clean
