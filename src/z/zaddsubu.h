/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

static inline HEBI_ALWAYSINLINE
void
hebi_zaddsubu__(hebi_zptr r, hebi_zsrcptr a, uint64_t b, int as, int bs)
{
	const hebi_packet *ap;
	hebi_packet *rp;
	uint64_t c;
	size_t u;

	u = a->hz_used;

	if (((as > 0) && (bs > 0)) || ((as < 0) && (bs < 0))) {
		rp = hebi_zgrowcopyif__(r, u + 1, r == a);
		ap = a->hz_packs;
		if ((c = hebi_paddu(rp, ap, b, u)) != 0) {
			hebi_psetu(rp + u, c);
			u++;
		}
		r->hz_used = u;
		r->hz_sign = as;
		return;
	}

	ap = a->hz_packs;

	if ((u > 1) || (ap->hp_limbs64[0] > b) || (ap->hp_limbs64[1] != 0)) {
		rp = r == a ? r->hz_packs : hebi_zgrow__(r, u);
		(void)hebi_psubu(rp, ap, b, u);
		u = hebi_pnorm(rp, u);
		r->hz_used = u;
		r->hz_sign = as;
	} else {
		c = b - ap->hp_limbs64[0];
		if (LIKELY(c)) {
			rp = hebi_zgrow__(r, 1);
			hebi_psetu(rp, c);
			r->hz_used = 1;
			r->hz_sign = bs;
		} else {
			r->hz_sign = 0;
		}
	}
}
