#!/usr/bin/env yash
# report.sh: generate CSV and Markdown summaries from yash .trs files

set -Ceu

export LC_ALL=C

csv_file="${1:?csv output not specified}"
md_file="${2:?markdown output not specified}"
shift 2

if [ "$#" -eq 0 ]; then
    print -u2 'report.sh: no result files specified'
    exit 1
fi

full_pass_files=0
partial_pass_files=0
full_fail_files=0
total_ok=0
total_fail=0
total_skip=0

{
    printf 'file,status,rc,ok,fail,skip\n'

    for result_file do
        file_name="${result_file%.trs}.tst"
        file_name="${file_name##*/}"

        ok="$(grep -c '^%%% OK\[' "$result_file" || true)"
        error="$(grep -c '^%%% ERROR\[' "$result_file" || true)"
        skip="$(grep -c '^%%% SKIPPED:' "$result_file" || true)"
        fail="$((error + skip))"

        if [ "$fail" -eq 0 ]; then
            status='FULL_PASS'
            rc=0
            full_pass_files="$((full_pass_files + 1))"
        elif [ "$ok" -eq 0 ]; then
            status='FULL_FAIL'
            rc=2
            full_fail_files="$((full_fail_files + 1))"
        else
            status='PARTIAL_PASS'
            rc=1
            partial_pass_files="$((partial_pass_files + 1))"
        fi

        total_ok="$((total_ok + ok))"
        total_fail="$((total_fail + fail))"
        total_skip="$((total_skip + skip))"

        printf '%s,%s,%s,%s,%s,%s\n' \
            "$file_name" "$status" "$rc" "$ok" "$fail" "$skip"
    done
} >| "$csv_file"

{
    printf '# POSIX Test Report\n\n'

    while IFS=, read -r file_name status rc ok fail skip; do
        case "$status" in
            ('status')
                continue
                ;;
            ('FULL_PASS')
                mark='[x]'
                ;;
            ('PARTIAL_PASS')
                mark='[-]'
                ;;
            (*)
                mark='[ ]'
                ;;
        esac
        printf '%s %s rc=%s ok=%s fail=%s skip=%s\n' \
            "$mark" "$file_name" "$rc" "$ok" "$fail" "$skip"
    done < "$csv_file"

    printf '\n## Totals\n\n'
    printf -- '- FULL_PASS: %s\n' "$full_pass_files"
    printf -- '- PARTIAL_PASS: %s\n' "$partial_pass_files"
    printf -- '- FULL_FAIL: %s\n' "$full_fail_files"
    printf -- '- ok: %s\n' "$total_ok"
    printf -- '- fail: %s\n' "$total_fail"
    printf -- '- skip: %s\n' "$total_skip"
} >| "$md_file"
