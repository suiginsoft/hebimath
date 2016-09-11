/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zshl(hebi_zptr r, hebi_zsrcptr a, size_t b)
{
	size_t rn, au;
	int as;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	} else if (UNLIKELY(b > SIZE_MAX - HEBI_PACKET_BIT + 1)) {
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
	}

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zsetzero(r);
		return;
	}

	au = a->hz_used;
	rn = au + (b + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;
	if (UNLIKELY(rn < au))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);

	if (rn > r->hz_resv)
		hebi_zrealloc_copyif__(r, rn, r == a);

	r->hz_used = hebi_pshl(r->hz_packs, a->hz_packs, b, au);
	r->hz_sign = as;
}
