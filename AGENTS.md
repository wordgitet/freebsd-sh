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

- The **primary release-blocking gate** is a **licensed POSIX conformance
  test suite** maintained **out of tree** (not shipped with this
  repository). Run it from a private install; use its journals and
  pass/fail results to decide correctness. Do not commit suite sources,
  scenario files, or journal excerpts to this repo.
- The imported **yash POSIX suite** (`test-posix/yash`) is a **fast
  regression guardrail** while conformance work lands. When the
  licensed suite and yash disagree, **prefer the licensed suite** for
  product behavior unless you have confirmed a harness or environment
  limitation (missing locale, PTY, privileges, etc.).
- The imported **FreeBSD sh** regression suite is a tertiary guardrail
  for accidental breakage in areas both suites touch.
- When POSIX behavior conflicts with legacy FreeBSD behavior, POSIX
  wins by default.
- Use the **POSIX standard text** as the normative spec. Use **yash**
  and **dash** only as secondary behavioral datapoints, not as the
  primary model when they conflict with the licensed conformance gate.

### Licensed conformance testing (agents)

- Workflow and suite layout for local runs live in **private** paths
  (e.g. `private/`); keep them **gitignored** and out of public
  releases.
- In commit messages and public docs, say **“licensed POSIX conformance
  suite”** or **“private conformance run”**; avoid pasting assertion
  text, test scripts, or journal diffs.
- Classify failures as **shell bug**, **harness/env**, or **untested**
  using journal evidence locally; do not vendor harness fixes into
  `test-posix/`.

### Performance guardrails

- Preserve the current fast non-interactive execution paths unless a
  failing conformance or POSIX test proves a hot-path change is
  necessary.
- Prefer fixes that are conditional on interactive mode, job control,
  trap handling, or special builtin/error paths.
- Tiny regressions only are acceptable on hot paths. Redesign fixes
  that noticeably slow loops, simple commands, or process launch.

### Test workflow

- **Release gate:** run the full licensed conformance campaign locally
  (shell, utilities, and UPE scenarios as applicable). Record
  pass/fail/untested/unresolved totals from journals; do not commit
  the journals.
- **Iteration:** `make TESTEE=./src/neoash test-posix-report` for broad
  POSIX-facing regression (yash import).
- **Faster iteration:** `make TESTEE=./src/neoash test-posix-nosignal-report`
  when not touching signals or job control.
- **Extra guardrail:** `make TESTEE=./src/neoash test-posix-freebsd`
  after changes in `cd`, `wait`, parser/error behavior, or
  traps/job control.
