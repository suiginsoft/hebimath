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
	size_t m, n;
	int as, bs;

	if (UNLIKELY(a == b)) {
		hebi_zsetzero(r);
		return;
	} else if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zset(r, b);
		return;
	} else if (UNLIKELY(!(bs = b->hz_sign))) {
		hebi_zset(r, a);
		return;
	}

	m = a->hz_used;
	n = b->hz_used;

	if (m < n) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, m, n);
	}

	if (r != a && r->hz_resv < m)
		rp = hebi_zexpandcopyif__(r, m, r->hz_resv, r == b);
	else
		rp = r->hz_packs;

	ap = a->hz_packs;
	bp = b->hz_packs;

	hebi_pxor(rp, ap, bp, n);
	if (n < m)
		hebi_pcopy(rp+n, ap+n, m-n);
	else
		m = hebi_pnorm(rp, m);

	if (LIKELY(m)) {
		r->hz_used = m;
		r->hz_sign = (as ^ bs) < 0 ? -1 : 1;
	} else {
		r->hz_sign = 0;
	}
}
