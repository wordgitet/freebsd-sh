# Upstream provenance

This repo is a fresh standalone snapshot extracted from:

- Repository: `chimerautils`
- Commit: `66db3db163cf273feaba1db7d9eebb8f369bdf21`
- Commit date: `2026-02-11`
- Commit subject: `fix build with muon`

The shell sources were subsequently synchronized with:

- Commit: `7efe8c9e921b018fa165674ac4382cbe8ac96308`
- Commit date: `2026-07-03`
- Commit subject: `update to 15.1-RELEASE`

## Imported paths

- `src.freebsd/sh` -> `src/`
- `src.freebsd/sh/bltin/echo.c` -> `src/bltin/echo.c`
- `src.freebsd/miscutils/kill/kill.c` -> `src/bltin/kill.c`
- `src.freebsd/coreutils/printf/printf.c` -> `src/bltin/printf.c`
- `src.freebsd/coreutils/test/test.c` -> `src/bltin/test.c`
- `src.freebsd/compat/mktemp.c` -> `src/compat/mktemp.c`
- `src.freebsd/compat/setmode.c` -> `src/compat/setmode.c`
- `src.compat/strlfuncs.c` -> `src/compat/strlfuncs.c`
- `src.compat/signames.c` -> `src/compat/signames.c`
- `include/` -> `include/`

## Notes

- This extraction intentionally does not preserve upstream git history.
- The standalone repo builds only the shell binary and its manpage.
- Standalone `/bin/test` and `[` install targets are intentionally omitted.
- The 15.1 job-lifecycle update is not imported because it produces
  unsolicited completed-job notifications and regresses the POSIX signal
  suite.

## Vendored dependencies

- `vendor/libedit/` is libedit 20260512-3.1 from
  https://www.thrysoee.dk/editline/. It is BSD-licensed; see its bundled
  `COPYING` file. Neoash builds it as a private static library so that line
  editing is reproducible and does not require a host libedit installation.
