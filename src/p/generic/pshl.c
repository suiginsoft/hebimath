/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
size_t
hebi_pshl(hebi_packet *r, const hebi_packet *a, size_t b, size_t n)
{
	int bits;
	size_t i, j, rn, limbs, offset;
	LIMB *rl, q, s;
	const LIMB *al;

	if (UNLIKELY(!n))
		return 0;

	bits = (int)(b % LIMB_BIT);
	limbs = b / LIMB_BIT;
	offset = limbs % LIMB_PER_PACKET;

	rn = n + (b + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;
	rl = LIMB_PTR(r) + limbs;
	al = LIMB_PTR(a);

	i = rn * LIMB_PER_PACKET - limbs;
	while (i > n * LIMB_PER_PACKET)
		rl[--i] = 0;

	if (LIKELY(bits)) {
		q = 0;
		j = i - offset - 1;
		while (i > j) {
			s = al[--i];
			rl[i+1] = s >> (LIMB_BIT - bits) | q;
			q = s << bits;
			if (rl[i+1])
				goto shift_rest;
		}
		--rn;
	shift_rest:
		while (i > 0) {
			s = al[--i];
			rl[i+1] = s >> (LIMB_BIT - bits) | q;
			q = s << bits;
		}
		rl[0] = q;
	} else {
		j = i - (offset ? offset : LIMB_PER_PACKET);
		while (i > j) {
			s = al[--i];
			rl[i] = s;
			if (s)
				goto move_rest;
		}
		--rn;
	move_rest:
		(void)memmove(rl, al, i * sizeof(LIMB));
	}

	(void)memset(r, 0, limbs * sizeof(LIMB));
	return rn;
}
