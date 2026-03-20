TESTEE ?= ../../build/src/sh
TEST_TIMEOUT ?= 5
TEST_SHELL_FLAGS ?=
CC ?= cc
CFLAGS ?= -O2

UTIL_BINS = util/argv util/fds util/getenv util/readdir

.PHONY: test-posix count utils clean veryclean

test-posix: utils
	TEST_SHELL='$(abspath $(TESTEE))' \
	TEST_SHELL_FLAGS='$(TEST_SHELL_FLAGS)' \
	TEST_TIMEOUT='$(TEST_TIMEOUT)' \
	TEST_UTIL='$(abspath util)' \
	./shell_tests.sh

count:
	@printf "Benchmarks %3d\n" $$(ls shell/benchmark.*.test | wc -l)
	@printf "Builtins   %3d\n" $$(ls shell/builtin.*.test | wc -l)
	@printf "Semantics  %3d\n" $$(ls shell/semantics.*.test | wc -l)
	@printf "Sh         %3d\n" $$(ls shell/sh.*.test | wc -l)
	@printf "==============\n"
	@printf "Total      %3d\n" $$(ls shell/*.test | wc -l)

utils: $(UTIL_BINS)

util/argv: util/argv.c
	$(CC) $(CFLAGS) -o $@ $<

util/fds: util/fds.c
	$(CC) $(CFLAGS) -o $@ $<

util/getenv: util/getenv.c
	$(CC) $(CFLAGS) -o $@ $<

util/readdir: util/readdir.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(UTIL_BINS)

veryclean: clean
	rm -rf log
