/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Compile-time standards profile shared by all shell subsystems.
 *
 * A standards profile exposes only facilities from the selected POSIX and
 * XSI editions.  The native profile retains all shell extensions.
 */

#ifndef STANDARDS_H_
#define STANDARDS_H_

#ifndef NEOASH_POSIX_VERSION
#define NEOASH_POSIX_VERSION 0L
#endif

#ifndef NEOASH_XOPEN_VERSION
#define NEOASH_XOPEN_VERSION 0
#endif

#define NEOASH_POSIX_PROFILE(version) \
	(NEOASH_POSIX_VERSION == (version))
#define NEOASH_XSI_PROFILE(version) \
	(NEOASH_XOPEN_VERSION == (version))
#define NEOASH_STANDARDS_PROFILE \
	(NEOASH_POSIX_VERSION != 0L)
#define NEOASH_EXTENSIONS \
	(NEOASH_POSIX_VERSION == 0L)
#define NEOASH_POSIX_AT_LEAST(version) \
	(NEOASH_POSIX_VERSION >= (version))
#define NEOASH_HAS_POSIX_2024 \
	(NEOASH_EXTENSIONS || NEOASH_POSIX_AT_LEAST(202405L))
#define NEOASH_OLD_XSI \
	(NEOASH_XOPEN_VERSION == 600 || NEOASH_XOPEN_VERSION == 700)
#define NEOASH_XSI \
	(NEOASH_XOPEN_VERSION != 0)

#endif /* !STANDARDS_H_ */
