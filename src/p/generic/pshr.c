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
	unsigned int bits;
	size_t i, j, m, limbs;
	LIMB *rl, q, s;
	const LIMB *al;

	limbs = b / LIMB_BIT;
	if (UNLIKELY(n * LIMB_PER_PACKET <= limbs))
		return 0;

	bits = (unsigned int)(b % LIMB_BIT);
	m = n * LIMB_PER_PACKET - limbs;

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a) + limbs;

	if (LIKELY(bits)) {
		q = al[0] >> bits;
		for (i = 1; i < m; i++) {
			s = al[i];
			rl[i-1] = s << (LIMB_BIT - bits) | q;
			q = s >> bits;
		}
		rl[i-1] = q;
	} else {
		(void)memmove(rl, al, m * sizeof(LIMB));
	}
	
	j = m;
	i = m - LIMB_PER_PACKET;
	if ((limbs %= LIMB_PER_PACKET)) {
		i += limbs;
		for ( ; limbs > 0; limbs--)
			rl[m++] = 0;
	}

	m /= LIMB_PER_PACKET;
	for ( ; i < j; i++)
		if (rl[i])
			return m;

	return m - 1;
}
