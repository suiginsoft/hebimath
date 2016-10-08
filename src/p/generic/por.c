/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
void
hebi_por(hebi_packet *r, const hebi_packet *a, const hebi_packet *b, size_t n)
{
	LIMB *rl;
	const LIMB *al;
	const LIMB *bl;
	size_t i, nl;

	ASSERT(n > 0);

	rl = LIMB_PTR(r);
	al = LIMB_PTR(a);
	bl = LIMB_PTR(b);
	nl = n * LIMB_PER_PACKET;

	i = 0;
	do {
		rl[i] = al[i] | bl[i];
	} while (++i < nl);
}
