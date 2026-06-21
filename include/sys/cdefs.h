/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021-2023 Daniel Kolesa
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SYS_CDEFS_H
#define SYS_CDEFS_H

#if defined(__has_include)
# if __has_include(<features.h>)
#  include <features.h>
# endif
#endif

#if defined(__has_include_next)
# if __has_include_next(<sys/cdefs.h>)
#  include_next <sys/cdefs.h>
# else
#  define _SYS_CDEFS_COMPAT
# endif
#elif defined(__GLIBC__) || defined(__CYGWIN__) || defined(__APPLE__)
# include_next <sys/cdefs.h>
#else
# define _SYS_CDEFS_COMPAT
#endif

/* The Apple SDK defines __CAST_AWAY_QUALIFIER (and thus __DECONST) using
 * _Pragma inside a macro body.  GCC 16 rejects _Pragma when it appears
 * inside a macro argument expression (e.g. as a function argument).
 * Override both macros with a plain two-step cast when building with GCC. */
#if defined(__GNUC__) && !defined(__clang__)
# undef  __CAST_AWAY_QUALIFIER
# define __CAST_AWAY_QUALIFIER(variable, qualifier, type) ((type)(uintptr_t)(variable))
# undef  __DECONST
# define __DECONST(type, var) ((type)(uintptr_t)(var))
#endif

#ifdef _SYS_CDEFS_COMPAT
# ifdef  __cplusplus
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS }
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#ifndef __COPYRIGHT
#define __COPYRIGHT(x)
#endif
#ifndef __FBSDID
#define __FBSDID(x)
#endif
#ifndef __SCCSID
#define __SCCSID(x)
#endif
#ifndef __RCSID
#define __RCSID(x)
#endif

#ifndef __dead2
#define __dead2
#endif
/* GCC does not recognise the "printf0" format specifier used by the Apple SDK
 * __printf0like definition, so forcibly redefine it to a no-op when compiling
 * with GCC regardless of what the SDK may have already declared. */
#ifdef __GNUC__
# ifndef __clang__
#  undef  __printf0like
#  define __printf0like(x, y)
# endif
#endif
#ifndef __printf0like
#define __printf0like(x, y)
#endif
#ifndef __printflike
#define __printflike(x, y) __attribute__((format(printf, x, y)))
#endif
/* Use a two-step cast through uintptr_t so that GCC accepts the expansion
 * even when it appears adjacent to a #pragma token (GCC 16 rejects a
 * direct cast-expression before #pragma). */
#ifndef __DECONST
#define __DECONST(type, var) ((type)(uintptr_t)(var))
#endif
#ifndef __unused
#define __unused __attribute__((unused))
#endif
#ifndef __unreachable
#define __unreachable() __builtin_unreachable()
#endif

#endif
