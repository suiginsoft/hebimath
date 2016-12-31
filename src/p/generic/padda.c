/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_padda(
		hebi_packet *r,
		const hebi_packet *a,
		size_t rn,
		size_t an )
{
	LIMB *rl;
	const LIMB *al;
	LIMB sum, carry;
	size_t i, m, n;

	ASSERT(rn >= an);
	ASSERT(an > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	m = rn * LIMB_PER_PACKET;
	n = an * LIMB_PER_PACKET;

	carry = 0;
	i = 0;

	do {
		sum = rl[i] + al[i] + carry;
		carry = (sum < rl[i]) || (sum == rl[i] && carry);
		rl[i] = sum;
	} while (++i < n);

	for ( ; i < m; i++) {
		sum = rl[i] + carry;
		carry = sum < rl[i];
		rl[i] = sum;
	}

	return carry;
}
