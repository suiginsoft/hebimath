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

	if (UNLIKELY(!n))
		return 0;

	al = LIMB_PTR(a);

	for (r = 0, i = n * LIMB_PER_PACKET; i--; r += LIMB_BIT)
		if (al[i])
			return LIMB_CLZ(al[i]) + r;

	return r;
}
