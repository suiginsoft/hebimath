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
	size_t n;

	if (UNLIKELY(!(as = a->hz_sign) || !(bs = b->hz_sign))) {
		hebi_zsetzero(r);
		return;
	}

	n = MIN(a->hz_used, b->hz_used);

	if (r == a) {
		ap = rp = r->hz_packs;
		bp = b->hz_packs;
	} else if (r == b) {
		ap = rp = r->hz_packs;
		bp = a->hz_packs;
	} else {
		rp = hebi_zgrowzero__(r, n);
		ap = a->hz_packs;
		bp = b->hz_packs;
	}

	hebi_pand(rp, ap, bp, n);
	n = hebi_pnorm(rp, n);

	if (LIKELY(n)) {
		r->hz_used = n;
		r->hz_sign = as < 0 && bs < 0 ? -1 : 1;
	} else {
		r->hz_sign = 0;
	}
}
