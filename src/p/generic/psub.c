/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_psub(
		hebi_packet *r,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	LIMB *rl;
	const LIMB *al;
	const LIMB *bl;
	LIMB diff, borrow;
	size_t i, m, n;

	ASSERT(an >= bn);
	ASSERT(bn > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	bl = LIMB_PTR(b);
	m = an * LIMB_PER_PACKET;
	n = bn * LIMB_PER_PACKET;

	borrow = 0;
	i = 0;

	do {
		diff = al[i] - bl[i] - borrow;
		borrow = (diff > al[i]) || (diff == al[i] && borrow);
		rl[i] = diff;
	} while (++i < n);

	for ( ; i < m; i++) {
		diff = al[i] - borrow;
		borrow = diff > al[i];
		rl[i] = diff;
	}

	return borrow;
}
