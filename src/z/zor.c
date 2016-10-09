/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zor(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	const hebi_packet *sp;
	const hebi_packet *ap;
	const hebi_packet *bp;
	int as, bs;
	size_t m, n;

	if (UNLIKELY(!(as = a->hz_sign) || !(bs = b->hz_sign))) {
		r->hz_sign = 0;
		return;
	}

	if (b->hz_used < a->hz_used) {
		sp = a->hz_packs;
		m = a->hz_used;
		n = b->hz_used;
	} else {
		sp = b->hz_packs;
		m = b->hz_used;
		n = a->hz_used;
	}

	if (r == a) {
		ap = rp = r->hz_packs;
		bp = b->hz_packs;
	} else if (r == b) {
		bp = rp = r->hz_packs;
		ap = a->hz_packs;
	} else {
		rp = hebi_zgrowzero__(r, n);
		ap = a->hz_packs;
		bp = b->hz_packs;
	}

	hebi_por(rp, ap, bp, n);
	if (n < m)
		hebi_pcopy(rp+n, sp+n, m-n);

	r->hz_used = n;
	r->hz_sign = (as > 0 && bs > 0) ? 1 : -1;
}
