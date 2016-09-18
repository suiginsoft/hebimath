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
	size_t r;

	ASSERT(n > 0);

	al = LIMB_PTR(a);
	n *= LIMB_PER_PACKET;
	r = 0;

	do {
		if (al[n-1]) {
			r += LIMB_CLZ(al[n-1]);
			break;
		}
		r += LIMB_BIT;
	} while (--n);

	return r;
}
