# Upstream provenance

This repo is a fresh standalone snapshot extracted from:

- Repository: `chimerautils`
- Commit: `66db3db163cf273feaba1db7d9eebb8f369bdf21`
- Commit date: `2026-02-11`
- Commit subject: `fix build with muon`

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
