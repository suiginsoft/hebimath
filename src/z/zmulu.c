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
	size_t u;
	int s;

	s = a->hz_sign;
	if (!s || !b) {
		hebi_zsetzero(r);
		return;
	} else if (b == 1) {
		hebi_zset(r, a);
		return;
	}

	u = a->hz_used;
	p = hebi_zgrowcopyif__(r, u + 1, r == a);

	overflow = hebi_pmulu(p, a->hz_packs, b, u);
	if (overflow) {
		hebi_psetu(p + u, overflow);
		u++;
	}

	r->hz_used = u;
	r->hz_sign = s;
}
