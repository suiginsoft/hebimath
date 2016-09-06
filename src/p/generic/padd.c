/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_padd(
		hebi_packet *r,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	LIMB *rl;
	const LIMB *al;
	const LIMB *bl;
	LIMB sum, carry;
	size_t i;

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	bl = LIMB_PTR(b);

	carry = 0;
	i = 0;

	for ( ; i < bn * LIMB_PER_PACKET; i++) {
		sum = al[i] + bl[i] + carry;
		carry = (sum < al[i]) || (sum == al[i] && carry);
		rl[i] = sum;
	}

	for ( ; i < an * LIMB_PER_PACKET; i++) {
		sum = al[i] + carry;
		carry = sum < al[i];
		rl[i] = sum;
	}

	return carry;
}
