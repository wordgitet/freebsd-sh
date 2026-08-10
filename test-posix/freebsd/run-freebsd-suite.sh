#!/bin/sh
set -eu
TESTEE="${1:?testee not specified}"
TIMEOUT="${2:?timeout not specified}"
shift 2
SUITE_DIR=$(
    CDPATH= cd -- "$(dirname "$0")" && pwd
)

case "$TESTEE" in
/*)
    ;;
*)
    TESTEE=$(command -v "$TESTEE") || {
        echo "testee not found: $TESTEE" >&2
        exit 1
    }
    TESTEE_DIR=$(CDPATH= cd -- "$(dirname "$TESTEE")" && pwd)
    TESTEE=$TESTEE_DIR/$(basename "$TESTEE")
    ;;
esac

TEST_PATH=/bin:/usr/bin:/sbin:/usr/sbin
HOST_OS=$(uname -s 2>/dev/null || echo unknown)
AVAILABLE_LOCALES=$(locale -a 2>/dev/null || :)
HAS_KTRACE=0
HAS_LOADER=0
HAS_DE_LOCALE=0
HAS_NL_LOCALE=0
HAS_EN_LOCALE=0
JOB_CONTROL_TESTS=0

if command -v ktrace >/dev/null 2>&1; then
    HAS_KTRACE=1
fi
if [ -x /libexec/ld-elf.so.1 ]; then
    HAS_LOADER=1
fi
if printf '%s\n' "$AVAILABLE_LOCALES" |
    grep -F -x 'de_DE.ISO8859-1' >/dev/null 2>&1; then
    HAS_DE_LOCALE=1
fi
if printf '%s\n' "$AVAILABLE_LOCALES" |
    grep -F -x 'nl_NL.ISO8859-1' >/dev/null 2>&1; then
    HAS_NL_LOCALE=1
fi
if printf '%s\n' "$AVAILABLE_LOCALES" |
    grep -F -x 'en_US.US-ASCII' >/dev/null 2>&1; then
    HAS_EN_LOCALE=1
fi
if [ "${FREEBSD_SH_JOB_CONTROL:-0}" = 1 ]; then
    JOB_CONTROL_TESTS=1
fi

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
skip_env=0
skip_dialect=0
fail=0

skip_test() {
    test_name=$1
    case "$test_name" in
    builtins/case3.0)
        printf 'SKIP-DIALECT %s (FreeBSD-specific backslash pattern semantics)\n' \
            "$test_name"
        skip_dialect=$((skip_dialect + 1))
        ;;
    builtins/case7.0)
        [ "$HAS_DE_LOCALE" -eq 1 ] && return 1
        printf 'SKIP-ENV %s (missing de_DE.ISO8859-1 locale)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    builtins/command3.0|builtins/command5.0|builtins/command6.0)
        printf 'SKIP-DIALECT %s (native command -V dialect differs from FreeBSD sh)\n' \
            "$test_name"
        skip_dialect=$((skip_dialect + 1))
        ;;
    builtins/command7.0|builtins/type2.0)
        [ "$HAS_LOADER" -eq 1 ] && return 1
        printf 'SKIP-ENV %s (missing FreeBSD /libexec/ld-elf.so.1)\n' \
            "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    builtins/kill2.0|builtins/wait11.0)
        [ "$JOB_CONTROL_TESTS" -eq 1 ] && return 1
        printf 'SKIP-ENV %s (job-control PTY not enabled)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    builtins/locale1.0)
        [ "$HAS_DE_LOCALE" -eq 1 ] && [ "$HAS_NL_LOCALE" -eq 1 ] &&
            return 1
        printf 'SKIP-ENV %s (missing required locale set)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    builtins/unalias.0)
        printf 'SKIP-DIALECT %s (FreeBSD-specific undefined-alias diagnostics)\n' \
            "$test_name"
        skip_dialect=$((skip_dialect + 1))
        ;;
    errors/assignment-error1.0|errors/assignment-error2.0)
        printf 'SKIP-DIALECT %s (FreeBSD-specific readonly-assignment behavior)\n' \
            "$test_name"
        skip_dialect=$((skip_dialect + 1))
        ;;
    execution/shellproc7.0)
        [ "$HOST_OS" = FreeBSD ] && return 1
        printf 'SKIP-ENV %s (FreeBSD APE execution test)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    expansion/pathname6.0)
        [ "$HAS_EN_LOCALE" -eq 1 ] && return 1
        printf 'SKIP-ENV %s (missing en_US.US-ASCII locale)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    expansion/plus-minus3.0)
        printf 'SKIP-DIALECT %s (POSIX leaves this quoting behavior undefined)\n' \
            "$test_name"
        skip_dialect=$((skip_dialect + 1))
        ;;
    parameters/mail1.0)
        [ "$HAS_KTRACE" -eq 1 ] && return 1
        printf 'SKIP-ENV %s (ktrace is unavailable)\n' "$test_name"
        skip_env=$((skip_env + 1))
        ;;
    *)
        return 1
        ;;
    esac
    return 0
}

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
    if skip_test "$test_name"; then
        continue
    fi
    workdir=$(mktemp -d "${TMPDIR:-/tmp}/freebsd-sh-test.XXXXXX")
    stdout_file=$workdir/stdout
    stderr_file=$workdir/stderr
    shell_link=$workdir/sh
    expected_status=${test_path##*.}
    expected_stdout=${test_path}.stdout
    expected_stderr=${test_path}.stderr
    actual_status=0
    (
        cd "$workdir"
        ln -s "$TESTEE" "$shell_link"
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
            PATH="$TEST_PATH" \
            SH="$shell_link" TESTEE="$shell_link" LC_ALL=C LANG=C \
            "$TIMEOUT_CMD" "$TIMEOUT" "$shell_link" "$test_path"
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
printf 'TOTAL:   %5d\n' $((pass + skip_env + skip_dialect + fail))
printf 'PASS:    %5d\n' "$pass"
printf 'SKIP-ENV:     %5d\n' "$skip_env"
printf 'SKIP-DIALECT: %5d\n' "$skip_dialect"
printf 'FAIL:    %5d\n' "$fail"
printf '==============\n'
if [ "$fail" -ne 0 ]; then
    exit 1
fi
