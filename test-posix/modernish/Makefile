# Modernish shell diagnostic suite

TESTEE ?= ../../src/neoash
TESTEE_FLAGS ?=
MODERNISH_TEST_FLAGS ?= -qq

.PHONY: test-posix

test-posix:
	MSH_SHELL='$(TESTEE)' $(TESTEE) $(TESTEE_FLAGS) bin/modernish --test $(MODERNISH_TEST_FLAGS)
