/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_pneg(hebi_packet *r, const hebi_packet *a, size_t n)
{
	LIMB *rl;
	const LIMB *al;
	LIMB diff, borrow;
	size_t i;

	ASSERT(n > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	n *= LIMB_PER_PACKET;

	borrow = 0;
	i = 0;

	do {
		diff = 0 - al[i] - borrow;
		borrow = (diff > 0) || (diff == 0 && borrow);
		rl[i] = diff;
	} while (++i < n);

	return borrow;
}
