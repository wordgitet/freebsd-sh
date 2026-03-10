#!/bin/sh

set -eu

die() {
    printf '%s\n' "$*" >&2
    exit 1
}

DIR=$(
    CDPATH= cd -- "$(dirname -- "$0")" && pwd
) || exit 1

YASH_RUNNER=${YASH_RUNNER:-}
if [ -z "$YASH_RUNNER" ]; then
    YASH_RUNNER=$(command -v yash || true)
fi
[ -n "$YASH_RUNNER" ] || die "run-posix.sh: yash not found in PATH"

CC_BIN=${CC:-cc}
TESTEE=${TESTEE:-"$DIR/../../build/src/sh"}

build_helper() {
    out=$1
    src=$2
    shift 2
    if [ ! -x "$DIR/$out" ] || [ "$DIR/$src" -nt "$DIR/$out" ]; then
        "$CC_BIN" -O2 -o "$DIR/$out" "$DIR/$src" "$@"
    fi
}

build_helper resetsig resetsig.c
build_helper checkfg checkfg.c
build_helper ptwrap ptwrap.c

cd "$DIR"

if [ "$#" -eq 0 ]; then
    set -- ./*-p.tst
fi

for test_file do
    "$DIR/resetsig" "$YASH_RUNNER" "$DIR/run-test.sh" "$TESTEE" "$test_file"
done

"$YASH_RUNNER" "$DIR/summarize.sh" ./*.trs
