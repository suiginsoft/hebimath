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
	uint64_t carry;
	size_t n, u;
	int s;

	if (UNLIKELY(!b)) {
		hebi_zset(r, a);
		return;
	} else if (UNLIKELY(!(s = a->hz_sign))) {
		hebi_zsetu(r, b);
		return;
	} 

	n = u = a->hz_used;
	if (s > 0)
		++n;

	if (n > r->hz_resv)
		hebi_zrealloc_copyif(r, n, r == a);

	rp = r->hz_packs;
	ap = a->hz_packs;

	if (s >= 0) {
		if ((carry = hebi_paddu(rp, ap, b, u)))
			hebi_psetu(rp + u++, carry);
	} else if (u > 1 || ap->hp_limbs64[0] > b || hebi_pcmpgtui64max(ap)) {
		(void)hebi_psubu(rp, ap, b, u);
		u = hebi_pnorm(rp, u);
	} else {
		rp->hp_limbs64[0] = b - ap->hp_limbs64[0];
		u = s = rp->hp_limbs64[0] ? 1 : 0;
	}

	r->hz_used = u;
	r->hz_sign = s;
}
