/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Lehmer RNG that generates alternating positive and negative integers
 * from seed 's' that is bounded by 2^(16(n/2+1)) with the top bit
 * always set.
 */

static char bczrand[] =
"define zrand(s, n) {\n"
"	auto p, r, m, i\n"
"	p = 1\n"
"	if (n % 2 == 1)\n"
"		p = -1\n"
"	n /= 2\n"
"	r = s\n"
"	for (i = 0; i < n; i++) {\n"
"	    r = r * 48271\n"
"	}\n"
"	m = 2^(16*(n+1)-1)\n"
"	r %= m\n"
"	r += m\n"
"	return p * r\n"
"}\n";

static int initialized;
static int seeds[4] = { 61937, 57397, 39769, 41777 };

void
zpermutation(long x, long n, int p, ...)
{
	hebi_zptr z;
	va_list ap;
	char *s;
	long y;
	int i;

	assert(x >= 0);
	assert(n >= 0);
	assert(p >= 0);
	assert(check_scale_perm > 0);

	if (!initialized) {
		bcwrite(bczrand);
		initialized = 1;
	}

	va_start(ap, p);
	for (i = 0; i < p; i++) {
		y = x % n;
		x = x / n;
		z = va_arg(ap, hebi_zptr);
		if (y < check_num_i64values) {
			hebi_zseti(z, check_i64values[y]);
		} else if (y < check_num_i64values + check_num_u64values - 3) {
			hebi_zsetu(z, check_u64values[y - check_num_i64values + 3]);
		} else {
			y -= check_num_i64values + check_num_u64values - 3;
			y *= check_scale_perm;
			s = bcprintf("zrand(%d,%ld)", seeds[i%4], y);
			hebi_zsetstr(z, s, NULL, 10);
			free(s);
		}
	}
	va_end(ap);
}
