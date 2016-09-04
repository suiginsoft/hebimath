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
	size_t i;

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	bl = LIMB_PTR(b);

	borrow = 0;
	i = 0;

	for ( ; i < bn * LIMB_PER_PACKET; ++i) {
		diff = al[i] - bl[i] - borrow;
		borrow = (diff > al[i]) || (diff == al[i] && borrow);
		rl[i] = diff;
	}

	for ( ; i < an * LIMB_PER_PACKET; ++i) {
		diff = al[i] - borrow;
		borrow = diff > al[i];
		rl[i] = diff;
	}

	return borrow;
}
