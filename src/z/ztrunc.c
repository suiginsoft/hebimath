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
	unsigned int b;
	size_t i;
	size_t n;
	size_t u;
	int s;

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
		r->hz_used = u;
		r->hz_sign = s;
	}

	if (LIKELY(u >= n && (b = (unsigned int)(bits % HEBI_PACKET_BIT)))) {
		rp += u - 1;
		if (LIKELY(b)) {
			b = HEBI_PACKET_BIT - b;
			i = 1;
			if (b >= 64) {
				rp->hp_limbs64[i] = 0;
				b -= 64;
				i--;
			}
			rp->hp_limbs64[i] &= (UINT64_C(1) << (64 - b)) - 1;
		} else {
			rp->hp_limbs64[0] = 0;
			rp->hp_limbs64[1] = 1;
		}
	}
}
