/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zshl(hebi_zptr r, hebi_zsrcptr a, size_t b)
{
	hebi_packet *rp;
	size_t rn, au;
	int as;

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zsetzero(r);
		return;
	} else if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	} else if (UNLIKELY(b > HEBI_PACKET_MAXLEN * HEBI_PACKET_BIT)) {
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
	}

	au = a->hz_used;
	rn = au + (b + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;

	if (UNLIKELY(rn < au || rn > HEBI_PACKET_MAXLEN))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	rp = hebi_zgrowcopyif__(r, rn, r == a);
	r->hz_used = hebi_pshl(rp, a->hz_packs, b, au);
	r->hz_sign = as;
}
