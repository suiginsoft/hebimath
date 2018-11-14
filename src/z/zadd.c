/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "zaddsub.h"

HEBI_API
void
hebi_zadd(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	int as;
	int bs;

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zset(r, b);
		return;
	}

	if (UNLIKELY(!(bs = b->hz_sign))) {
		hebi_zset(r, a);
		return;
	}
	
	hebi_zaddsub__(r, a, b, as, bs);
}
