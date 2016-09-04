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

	rn = n - rn;
	rl = LIMB_PTR(r);
	al = LIMB_PTR(a) + limbs;

	if (LIKELY(bits)) {
		q = al[0] >> bits;
		for (i = 1; i < rn * LIMB_PER_PACKET; i++) {
			s = al[i];
			rl[i-1] = s << (LIMB_BIT - bits) | q;
			q = s >> bits;
		}
		rl[i-1] = q;
	} else {
		(void)memmove(rl, al, rn * sizeof(hebi_packet));
		i = rn * LIMB_PER_PACKET;
	}

	if ((j = i & ~(LIMB_PER_PACKET - 1)) != i)
	{
		k = i;
		for (limbs %= LIMB_PER_PACKET; limbs > 0; limbs--)
			rl[k++] = 0;
	} else {
		j -= LIMB_PER_PACKET;
	}

	for ( ; j < i; j++)
		if (rl[j])
			return rn;

	return rn - 1;
}
