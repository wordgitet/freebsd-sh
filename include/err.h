/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Daniel Kolesa
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

#ifndef ERR_H
#define ERR_H

#include "config-compat.h"

#ifdef HAVE_ERR_H
#include_next <err.h>
#else

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifndef HAVE_GETPROGNAME
extern const char *compat_getprogname(void);
#define getprogname compat_getprogname
#endif

static inline void verr(int eval, const char *fmt, va_list ap) {
    int save_errno = errno;
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(save_errno));
    exit(eval);
}

static inline void err(int eval, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verr(eval, fmt, ap);
    va_end(ap);
}

static inline void verrx(int eval, const char *fmt, va_list ap) {
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        vfprintf(stderr, fmt, ap);
    }
    fprintf(stderr, "\n");
    exit(eval);
}

static inline void errx(int eval, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    va_end(ap);
}

static inline void vwarn(const char *fmt, va_list ap) {
    int save_errno = errno;
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(save_errno));
}

static inline void warn(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

static inline void vwarnx(const char *fmt, va_list ap) {
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        vfprintf(stderr, fmt, ap);
    }
    fprintf(stderr, "\n");
}

static inline void warnx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarnx(fmt, ap);
    va_end(ap);
}

#endif /* HAVE_ERR_H */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_ERRC
static inline void errc(int eval, int code, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(code));
    exit(eval);
}
#endif

#ifndef HAVE_WARNC
static inline void warnc(int code, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "%s: ", getprogname());
    if (fmt != NULL) {
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(code));
}
#endif

#ifdef __cplusplus
}
#endif

#endif
