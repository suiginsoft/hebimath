/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
size_t
hebi_pshr(hebi_packet *r, const hebi_packet *a, size_t b, size_t n)
{
	int bits;
	size_t i, j, k, rn, limbs;
	LIMB *rl, q, s;
	const LIMB *al;

	rn = b / HEBI_PACKET_BIT;
	if (UNLIKELY(n <= rn))
		return 0;

	bits = (int)(b % LIMB_BIT);
	limbs = b / LIMB_BIT;

	rl = LIMB_PTR(r);
	rn = n - rn;

	al = LIMB_PTR(a) + limbs;
	limbs = limbs % LIMB_PER_PACKET;
	j = rn * LIMB_PER_PACKET - limbs;

	if (LIKELY(bits)) {
		q = al[0] >> bits;
		for (i = 1; i < j; i++) {
			s = al[i];
			rl[i-1] = s << (LIMB_BIT - bits) | q;
			q = s >> bits;
		}
		rl[i-1] = q;
	} else {
		(void)memmove(rl, al, j * sizeof(LIMB));
	}

	if ((i = j & ~(LIMB_PER_PACKET - 1)) != j) {
		for (k = j; limbs > 0; limbs--)
			rl[k++] = 0;
	} else {
		i -= LIMB_PER_PACKET;
	}

	for ( ; i < j; i++)
		if (rl[i])
			return rn;

	return rn - 1;
}
