# freebsd-sh agent notes

This file contains repo-specific instructions for agents working in this
tree.

## Commit style

Use Linux-kernel-style commit messages, but distinguish between normal
commits and merge commits.

### Normal commits

Format:

```text
subsystem: imperative summary

Explain what changed and why.

Add more detail in wrapped paragraphs when needed.

Areas:
  subsystem: short note for another touched area
  subsystem: short note for another touched area

Signed-off-by: wordgitet <wordatet@linuxmail.org>
```

Rules:

- Use a subsystem-prefixed subject.
- Use imperative mood.
- Do not end the subject with a period.
- Keep the subject short.
- Leave one blank line after the subject.
- Wrap body text to about 72 columns.
- The body should explain both what changed and why.
- If the commit touches multiple meaningful areas, end the body with an
  `Areas:` section.
- Keep `Signed-off-by:` as the last trailer.
- Optional trailers such as `Test:` or `Upstream:` may appear above the
  sign-off when useful.

Example:

```text
tests: import yash POSIX suite

Import the POSIX-facing yash tests under test-posix/yash and add a local
runner that requires yash from PATH.

Exclude *-y.tst files because they cover yash-specific behavior rather than
generic POSIX shell semantics.

Areas:
  tests: vendor yash POSIX test files into test-posix/yash
  harness: run the imported test scripts with yash from PATH
  docs: document the local runner behavior

Signed-off-by: wordgitet <wordatet@linuxmail.org>
```

### Merge commits

Use the kernel-style merge summary format for merge commits.

Format:

```text
Merge branch 'topic-name'

Merge topic-name updates:

 - summary bullet
 - summary bullet
 - summary bullet

* branch 'topic-name':
  subsystem: first commit subject
  subsystem: second commit subject
  subsystem: third commit subject
```

For pulled tags or remote branches, use:

```text
Merge tag 'tag-name' of <repo-url>

Pull <topic> updates:

 - summary bullet
 - summary bullet

* tag 'tag-name' of <repo-url>:
  subsystem: first commit subject
  subsystem: second commit subject
```

Rules:

- Do not add `Signed-off-by:` to merge commits unless explicitly required.
- Summarize the merged branch or tag in short bullets.
- List the merged commit subjects at the end.
- Keep the tone close to the Linux kernel examples.

### Preference

If a commit is not a merge commit, use the normal commit format above.

### Versioning

Every commit that modifies code or build configurations must bump the project version (e.g., from `15.0-verX` to `15.0-verY` in `configure.ac`) as part of the same commit. The only exceptions are commits that solely update documentation, tests, or other non-code-related assets.

## Compliance strategy

This project is POSIX-first.

- The imported yash POSIX suite is the primary release-blocking
  correctness gate.
- The imported FreeBSD sh regression suite is a secondary guardrail
  used to catch accidental breakage while POSIX fixes land.
- When POSIX behavior conflicts with legacy FreeBSD behavior, POSIX
  wins by default.
- Use yash behavior and the POSIX text as the main semantic reference.
  Dash may be used as a secondary datapoint, but not as the primary
  model.

### Performance guardrails

- Preserve the current fast non-interactive execution paths unless a
  failing POSIX test proves a hot-path change is necessary.
- Prefer fixes that are conditional on interactive mode, job control,
  trap handling, or special builtin/error paths.
- Tiny regressions only are acceptable on hot paths. Redesign fixes
  that noticeably slow loops, simple commands, or process launch.

### Test workflow

- Use `make TESTEE=./src/neoash test-posix-report` as the main
  correctness scoreboard.
- Use `make TESTEE=./src/neoash test-posix-nosignal-report` for
  faster iteration when working outside signals and job control.
- Use `make TESTEE=./src/neoash test-posix-freebsd` as a secondary
  regression pass after changes in `cd`, `wait`, parser/error
  behavior, or traps/job control.
