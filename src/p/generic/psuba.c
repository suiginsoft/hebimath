/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_psuba(
		hebi_packet *r,
		const hebi_packet *a,
		size_t rn,
		size_t an )
{
	LIMB *rl;
	const LIMB *al;
	LIMB diff, borrow;
	size_t i, m, n;

	ASSERT(rn >= an);
	ASSERT(an > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	m = rn * LIMB_PER_PACKET;
	n = an * LIMB_PER_PACKET;

	borrow = 0;
	i = 0;

	do {
		diff = rl[i] - al[i] - borrow;
		borrow = (diff > rl[i]) || (diff == rl[i] && borrow);
		rl[i] = diff;
	} while (++i < n);

	for ( ; i < m; i++) {
		diff = rl[i] - borrow;
		borrow = diff > rl[i];
		rl[i] = diff;
	}

	return borrow;
}
