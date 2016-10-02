/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pclz(const hebi_packet *a, size_t n)
{
	const LIMB *al;
	size_t i, r;

	ASSERT(n > 0);

	al = LIMB_PTR(a);
	i = n * LIMB_PER_PACKET - 1;
	r = 0;

	do {
		if (al[i]) {
			r += LIMB_CLZ(al[i]);
			break;
		}
		r += LIMB_BIT;
	} while (i--);

	return r;
}
