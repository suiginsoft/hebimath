/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_ztrunc(hebi_zptr r, hebi_zsrcptr a, size_t bits)
{
	hebi_packet *rp;
	size_t n, u;
	int b, s;

	if (UNLIKELY(!(s = a->hz_sign) || !bits)) {
		r->hz_sign = 0;
		return;
	}

	n = (bits + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;
	u = MIN(n, a->hz_used);

	if (r == a) {
		rp = r->hz_packs;
		r->hz_used = u;
	} else {
		rp = hebi_zgrow__(r, u);
		hebi_pcopy(rp, a->hz_packs, u);
		r->hz_sign = s;
		r->hz_used = u;
	}

	if (LIKELY(u >= n && (b = (int)(bits % HEBI_PACKET_BIT))))
		hebi_pandmsk__(rp + u - 1, b);
}
