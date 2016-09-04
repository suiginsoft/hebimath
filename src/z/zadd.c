/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zadd(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	uint64_t carry;
	size_t au, bu;
	int as, bs, d;

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zset(r, b);
		return;
	} else if (UNLIKELY(!(bs = b->hz_sign))) {
		hebi_zset(r, a);
		return;
	}

	au = a->hz_used;
	bu = b->hz_used;
	if (au < bu) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, au, bu);
		SWAP(int, as, bs);
	}

	if ((as ^ bs) >= 0) {
		rp = hebi_zgrow_copyif(r, au + 1, r == a || r == b);
		if ((carry = hebi_padd(rp, a->hz_packs, b->hz_packs, au, bu)))
			hebi_psetu(rp + au++, carry);
	} else {
		if (au == bu) {
			d = hebi_pcmp(a->hz_packs, b->hz_packs, au);
			if (d <= 0) {
				if (UNLIKELY(!d)) {
					hebi_zsetzero(r);
					return;
				}
				SWAP(hebi_zsrcptr, a, b);
				as = -as;
			}
		}
		rp = hebi_zgrow_copyif(r, au, r == a || r == b);
		(void)hebi_psub(rp, a->hz_packs, b->hz_packs, au, bu);
		au = hebi_pnorm(rp, au);
	}

	r->hz_used = au;
	r->hz_sign = as;
}
