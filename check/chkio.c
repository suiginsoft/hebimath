/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int
vsnchkprintf(char *restrict s, size_t n, const char *restrict fmt, va_list ap)
{
	size_t f = 0, i = 0, k;
	int c, j, ll;

	assert(s);
	assert(fmt);

	for (;;) {
		c = fmt[f++];
		if (c == '\0') {
			break;
		} else if (c != '%') {
			if (i < n)
				s[i] = (char)c;
			++i;
			continue;
		}
		k = n > i ? n-i : 0;
		j = ll = 0;
	restart:
		c = fmt[f++];
		switch (c) {
		case '%':
			if (k > 0) {
				s[i] = '%';
				j = 1;
			}
			break;
		case 'l':
			if (++ll > 2)
				fail("bad format string");
			goto restart;
		case 'i':
		case 'd':
			if (ll == 0)
				j = snprintf(s+i, k, "%d", va_arg(ap, int));
			else if (ll == 1)
				j = snprintf(s+i, k, "%ld", va_arg(ap, long));
			else
				j = snprintf(s+i, k, "%lld", va_arg(ap, long long));
			break;
		case 'u':
			if (ll == 0)
				j = snprintf(s+i, k, "%u", va_arg(ap, unsigned int));
			else if (ll == 1)
				j = snprintf(s+i, k, "%lu", va_arg(ap, unsigned long));
			else
				j = snprintf(s+i, k, "%llu", va_arg(ap, unsigned long long));
			break;
		case 's':
			j = snprintf(s+i, k, "%s", va_arg(ap, const char*));
			break;
		case 'z':
			if ((ll += (int)(sizeof(size_t) / sizeof(long))) > 2)
				fail("bad format string");
			goto restart;
		case 'Z':
			if (ll != 0)
				fail("bad format string");
			j = (int)hebi_zgetstr(s+i, k, va_arg(ap, hebi_zsrcptr), 10, 0);
			break;
		default:
			fail("bad format string");
			break;
		}
		assert(j >= 0);
		i += (size_t)j;
	}

	if (i < n)
		s[i] = '\0';
	else if (n)
		s[n-1] = '\0';
	return i < INT_MAX ? (int)i : INT_MAX;
}

int
vaschkprintf(char **restrict strp, const char *restrict fmt, va_list ap)
{
	va_list ap2;
	char *buf = NULL;
	size_t n = 1024, bufsz = 0;
	int r;
	
	assert(strp);
	assert(fmt);

	do {
		if (n >= bufsz) {
			bufsz = MAX(n + 1, bufsz * 2);
			buf = realloc(buf, bufsz);
		}
		va_copy(ap2, ap);
		r = vsnchkprintf(buf, bufsz, fmt, ap2);
		va_end(ap2);
		if (r < 0) {
			free(buf);
			return r;
		}
		n = (size_t)r;
	} while (n >= bufsz);

	*strp = buf;
	return r;
}

int
aschkprintf(char **restrict strp, const char *restrict fmt, ...)
{
	va_list ap;
	int r;

	assert(strp);
	assert(fmt);

	va_start(ap, fmt);
	r = vaschkprintf(strp, fmt, ap);
	va_end(ap);
	return r;
}
