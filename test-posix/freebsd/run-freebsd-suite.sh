#!/bin/sh
set -eu
TESTEE="${1:?testee not specified}"
TIMEOUT="${2:?timeout not specified}"
shift 2
SUITE_DIR=$(
    CDPATH= cd -- "$(dirname "$0")" && pwd
)

# Detect GNU vs BSD find: GNU find understands --version, BSD find does not.
if find --version >/dev/null 2>&1; then
    FIND_IS_GNU=1
else
    FIND_IS_GNU=0
fi

# Detect a usable timeout(1): GNU coreutils ships timeout; on macOS without
# coreutils installed, it's only available as gtimeout via Homebrew.
if command -v timeout >/dev/null 2>&1; then
    TIMEOUT_CMD=timeout
elif command -v gtimeout >/dev/null 2>&1; then
    TIMEOUT_CMD=gtimeout
else
    echo "no timeout command available (install GNU coreutils, e.g. 'brew install coreutils')" >&2
    exit 1
fi

if [ "$#" -gt 0 ]; then
    TEST_LIST="$*"
else
    TEST_LIST=$(
        cd "$SUITE_DIR"
        if [ "$FIND_IS_GNU" -eq 1 ]; then
            find builtins errors execution expansion invocation parameters parser set-e \
                -type f -regextype posix-extended -regex '.*\.[0-9]+$'
        else
            find -E builtins errors execution expansion invocation parameters parser set-e \
                -type f -regex '.*\.[0-9]+$'
        fi | LC_ALL=C sort
    )
fi
if [ -z "$TEST_LIST" ]; then
    echo "No FreeBSD sh tests selected" >&2
    exit 1
fi
pass=0
fail=0
compare_output() {
    expected_file=$1
    actual_file=$2
    label=$3
    if [ -f "$expected_file" ]; then
        if cmp -s "$expected_file" "$actual_file"; then
            return 0
        fi
        printf '%s mismatch\n' "$label" >&2
        diff -u "$expected_file" "$actual_file" >&2 || true
        return 1
    fi
    if [ ! -s "$actual_file" ]; then
        return 0
    fi
    printf '%s mismatch\n' "$label" >&2
    diff -u /dev/null "$actual_file" >&2 || true
    return 1
}
for test_rel in $TEST_LIST; do
    case "$test_rel" in
    /*)
        test_path=$test_rel
        case "$test_path" in
        "$SUITE_DIR"/*)
            test_name=${test_path#"$SUITE_DIR"/}
            ;;
        *)
            test_name=$(basename "$test_path")
            ;;
        esac
        ;;
    *)
        test_name=$test_rel
        test_path=$SUITE_DIR/$test_rel
        ;;
    esac
    if [ ! -f "$test_path" ]; then
        printf 'FAIL %s\n' "$test_name" >&2
        printf 'missing test file: %s\n' "$test_path" >&2
        fail=$((fail + 1))
        continue
    fi
    workdir=$(mktemp -d "${TMPDIR:-/tmp}/freebsd-sh-test.XXXXXX")
    stdout_file=$workdir/stdout
    stderr_file=$workdir/stderr
    expected_status=${test_path##*.}
    expected_stdout=${test_path}.stdout
    expected_stderr=${test_path}.stderr
    actual_status=0
    (
        cd "$workdir"
        env \
            -u LANGUAGE \
            -u LANG \
            -u LC_ALL \
            -u LC_COLLATE \
            -u LC_CTYPE \
            -u LC_MESSAGES \
            -u LC_MONETARY \
            -u LC_NUMERIC \
            -u LC_TIME \
            SH="$TESTEE" TESTEE="$TESTEE" LC_ALL=C LANG=C \
            "$TIMEOUT_CMD" "$TIMEOUT" "$TESTEE" "$test_path"
    ) >"$stdout_file" 2>"$stderr_file" || actual_status=$?
    test_failed=0
    if [ "$actual_status" -ne "$expected_status" ]; then
        printf 'exit status mismatch: expected=%s actual=%s\n' \
            "$expected_status" "$actual_status" >&2
        test_failed=1
    fi
    if ! compare_output "$expected_stdout" "$stdout_file" "stdout"; then
        test_failed=1
    fi
    if ! compare_output "$expected_stderr" "$stderr_file" "stderr"; then
        test_failed=1
    fi
    if [ "$test_failed" -eq 0 ]; then
        printf 'PASS %s\n' "$test_name"
        pass=$((pass + 1))
    else
        printf 'FAIL %s\n' "$test_name" >&2
        fail=$((fail + 1))
    fi
    rm -rf "$workdir"
done
printf '==============\n'
printf 'TOTAL:   %5d\n' $((pass + fail))
printf 'PASS:    %5d\n' "$pass"
printf 'FAIL:    %5d\n' "$fail"
printf '==============\n'
if [ "$fail" -ne 0 ]; then
    exit 1
fi
