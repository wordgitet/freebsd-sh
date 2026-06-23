/* source: musl libc and OpenBSD */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "config-compat.h"

#ifndef HAVE_STRLCPY
size_t strlcpy(char *d, const char *s, size_t n) {
    char *d0 = d;

    if (!n--) goto finish;

    for (; n && (*d = *s); n--, s++, d++);
    *d = 0;
finish:
    return d - d0 + strlen(s);
}
#endif

#ifndef HAVE_STRLCAT
size_t strlcat(char *d, const char *s, size_t n) {
    size_t l = strnlen(d, n);
    if (l == n) return l + strlen(s);
    return l + strlcpy(d + l, s, n - l);
}
#endif

#ifndef HAVE_STRCHRNUL
char *strchrnul(const char *s, int c) {
    c = (unsigned char)c;
    while (*s && *s != c) {
        s++;
    }
    return (char *)s;
}
#endif

#ifndef HAVE_ASPRINTF
int vasprintf(char **strp, const char *fmt, va_list ap) {
    va_list ap2;
    int len;
    char *buf;

    va_copy(ap2, ap);
    len = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);

    if (len < 0) return -1;
    buf = malloc((size_t)len + 1);
    if (!buf) return -1;
    vsnprintf(buf, (size_t)len + 1, fmt, ap);
    *strp = buf;
    return len;
}

int asprintf(char **strp, const char *fmt, ...) {
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vasprintf(strp, fmt, ap);
    va_end(ap);
    return ret;
}
#endif
