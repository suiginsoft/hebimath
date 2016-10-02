/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
int
hebi_pcmp(const hebi_packet *a, const hebi_packet *b, size_t n)
{
	const LIMB *al;
	const LIMB *bl;
	size_t i;

	ASSERT(n > 0);

	al = LIMB_PTR(a);
	bl = LIMB_PTR(b);
	i = n * LIMB_PER_PACKET - 1;

	do {
		if (al[i] < bl[i])
			return -1;
		else if (al[i] > bl[i])
			return 1;
	} while (i--);

	return 0;
}
