/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zmulu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	hebi_packet *p;
	uint64_t overflow;
	size_t n, u;
	int s;

	s = a->hz_sign;
	if (!s | !b) {
		hebi_zsetzero(r);
		return;
	} else if (b == 1) {
		hebi_zset(r, a);
		return;
	}

	n = u = a->hz_used;
	if (++n > r->hz_resv)
		hebi_zrealloc_copyif__(r, n, r == a);

	p = r->hz_packs;
	if ((overflow = hebi_pmulu(p, a->hz_packs, b, u)))
		hebi_psetu(p + u++, overflow);

	r->hz_used = u;
	r->hz_sign = s;
}
