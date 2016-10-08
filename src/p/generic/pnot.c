/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
void
hebi_pnot(hebi_packet *r, const hebi_packet *a, size_t n)
{
	LIMB *rl;
	const LIMB *al;
	size_t i, nl;

	ASSERT(n > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	nl = n * LIMB_PER_PACKET;

	i = 0;
	do {
		rl[i] = ~al[i];
	} while (++i < nl);
}
