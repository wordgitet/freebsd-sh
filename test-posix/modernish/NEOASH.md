# neoash vendor notes

This directory vendors modernish solely as a shell diagnostic and regression
suite. It is not a dependency of the neoash binary or its installed files.

Snapshot source: <https://github.com/modernish/modernish>

- Commit: `cde49fa37f93e943b9cb9b9536ce83836c4864ae`
- Commit date: 2026-02-12
- Commit subject: `Fixes to run on recent yash in POSIX mode`
- License: ISC; see `LICENSE`.

Run it from the repository root with:

    make TESTEE=./src/neoash test-posix-modernish

The runner sets `MSH_SHELL` to `TESTEE`, preventing modernish from choosing a
different shell from `PATH` during its startup probe.

Modernish diagnoses implementation bugs, quirks, and optional capabilities.
Its result is supplementary diagnostic evidence: the yash POSIX suite remains
neoash's release-blocking correctness gate.
