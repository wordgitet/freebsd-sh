SUBDIRS = src
EXTRA_DIST = README UPSTREAM.md AGENTS.md meson.build meson_options.txt \
    include/bitstring.h include/bsdhash.h include/capsicum_helpers.h \
    include/err.h include/libcasper.h include/libutil.h include/md5.h \
    include/regex.h include/ripemd.h include/sha.h include/sha224.h \
    include/sha256.h include/sha384.h include/sha512.h include/sha512t.h \
    include/signal.h include/stdlib.h include/string.h include/time_bsd.h \
    include/unistd.h include/utmpx.h include/wchar.h

TESTEE ?= $(top_builddir)/src/sh
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

# Clean the test files on standard clean
clean-local: clean-test-posix
