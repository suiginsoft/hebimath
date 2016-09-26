/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zsubu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	const hebi_packet *ap;
	hebi_packet *rp;
	uint64_t c;
	size_t n, u;
	int s;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	} else if (UNLIKELY(!(s = a->hz_sign))) {
		hebi_zsetu(r, b);
		hebi_zneg(r, r);
		return;
	} 

	n = u = a->hz_used;
	if (s < 0)
		++n;

	if (n > r->hz_resv)
		hebi_zrealloc_copyif__(r, n, r == a);

	rp = r->hz_packs;
	ap = a->hz_packs;

	if (s <= 0) {
		if ((c = hebi_paddu(rp, ap, b, u)))
			hebi_psetu(rp + u++, c);
	} else if (u > 1 || ap->hp_limbs64[0] > b || hebi_pcmpgtui64max__(ap)) {
		(void)hebi_psubu(rp, ap, b, u);
		u = hebi_pnorm(rp, u);
	} else {
		c = b - ap->hp_limbs64[0];
		hebi_psetu(rp, c);
		u = c ? 1 : 0;
		s = c ? -1 : 0;
	}

	r->hz_used = u;
	r->hz_sign = s;
}
