/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zand(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	const hebi_packet *ap;
	const hebi_packet *bp;
	int as, bs;
	size_t u;

	if (UNLIKELY(!(as = a->hz_sign) || !(bs = b->hz_sign))) {
		r->hz_sign = 0;
		return;
	}

	u = MIN(a->hz_used, b->hz_used);

	if (r == a) {
		ap = rp = r->hz_packs;
		bp = b->hz_packs;
	} else if (r == b) {
		bp = rp = r->hz_packs;
		ap = a->hz_packs;
	} else {
		rp = hebi_zgrowzero__(r, u);
		ap = a->hz_packs;
		bp = b->hz_packs;
	}

	hebi_pand(rp, ap, bp, u);
	u = hebi_pnorm(rp, u);

	if (LIKELY(u)) {
		r->hz_used = u;
		r->hz_sign = (as > 0 || bs > 0) ? 1 : -1;
	} else {
		r->hz_sign = 0;
	}
}
