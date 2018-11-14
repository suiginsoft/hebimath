/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "zaddsubu.h"

HEBI_API
void
hebi_zsubu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	int as;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	}

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zsetu(r, b);
		hebi_zneg(r, r);
		return;
	}

	hebi_zaddsubu__(r, a, b, as, -1);
}
