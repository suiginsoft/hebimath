/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zxor(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	const hebi_packet *ap;
	const hebi_packet *bp;
	size_t an;
	size_t bn;
	int as;
	int bs;

	if (UNLIKELY(a == b)) {
		hebi_zsetzero(r);
		return;
	}

	as = a->hz_sign;
	if (UNLIKELY(!as)) {
		hebi_zset(r, b);
		return;
	}

	bs = b->hz_sign;
	if (UNLIKELY(!bs)) {
		hebi_zset(r, a);
		return;
	}

	an = a->hz_used;
	bn = b->hz_used;

	if (an < bn) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, an, bn);
	}

	if (r != a && r->hz_resv < an)
		rp = hebi_zexpandcopyif__(r, an, r->hz_resv, r == b);
	else
		rp = r->hz_packs;

	ap = a->hz_packs;
	bp = b->hz_packs;

	hebi_pxor(rp, ap, bp, bn);

	if (bn < an)
		hebi_pcopy(rp+bn, ap+bn, an-bn);
	else
		an = hebi_pnorm(rp, an);

	if (LIKELY(an)) {
		r->hz_used = an;
		r->hz_sign = SIGNXOR(as, bs) < 0 ? -1 : 1;
	} else {
		r->hz_sign = 0;
	}
}
