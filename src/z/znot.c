/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_znot(hebi_zptr r, hebi_zsrcptr a)
{
	hebi_packet *rp;
	size_t n;
	int s;

	if (UNLIKELY(!(s = a->hz_sign))) {
		hebi_zsetzero(r);
		return;
	}

	n = a->hz_used;
	rp = hebi_zgrowzero__(r, n);

	hebi_pnot(rp, a->hz_packs, n);
	n = hebi_pnorm(rp, n);

	if (LIKELY(n)) {
		r->hz_used = n;
		r->hz_sign = -s;
	} else {
		r->hz_sign = 0;
	}
}
