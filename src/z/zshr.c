/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zshr(hebi_zptr r, hebi_zsrcptr a, size_t b)
{
	size_t rn, au;
	int as;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	}

	as = a->hz_sign;
	au = a->hz_used;
	rn = b / HEBI_PACKET_BIT;
	if (UNLIKELY(!as || au <= rn)) {
		hebi_zsetzero(r);
		return;
	}

	rn = au - rn;
	if (rn > r->hz_resv)
		hebi_zrealloc_copyif__(r, rn, r == a);

	if (LIKELY(rn = hebi_pshr(r->hz_packs, a->hz_packs, b, au))) {
		r->hz_used = rn;
		r->hz_sign = as;
	} else {
		r->hz_sign = 0;
	}
}
