/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zaddu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	const hebi_packet *ap;
	hebi_packet *rp;
	uint64_t c;
	size_t u;
	int s;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	}

	if (UNLIKELY(!(s = a->hz_sign))) {
		hebi_zsetu(r, b);
		return;
	}

	u = a->hz_used;

	if (s > 0) {
		rp = hebi_zgrowcopyif__(r, u + 1, r == a);
		ap = a->hz_packs;
		if ((c = hebi_paddu(rp, ap, b, u)))
			hebi_psetu(rp + u++, c);
		r->hz_used = u;
		r->hz_sign = s;
		return;
	}

	ap = a->hz_packs;

	if (u > 1 || ap->hp_limbs64[0] > b || ap->hp_limbs64[1] != 0) {
		if (r == a)
			rp = r->hz_packs;
		else
			rp = hebi_zgrow__(r, u);
		(void)hebi_psubu(rp, ap, b, u);
		u = hebi_pnorm(rp, u);
		r->hz_used = u;
		r->hz_sign = s;
	} else {
		c = b - ap->hp_limbs64[0];
		if (LIKELY(c)) {
			rp = hebi_zgrow__(r, 1);
			hebi_psetu(rp, c);
			r->hz_used = 1;
			r->hz_sign = 1;
		} else {
			r->hz_sign = 0;
		}
	}
}
