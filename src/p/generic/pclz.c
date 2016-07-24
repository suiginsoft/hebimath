/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pclz(const hebi_packet *a, size_t n)
{
	const HALF *ah;
	size_t i, r;

	if (UNLIKELY(!n))
		return 0;

	ah = HALF_PTR(a);
	for (r = 0, i = n * HALF_PER_PACKET; i--; r += HALF_BITS)
		if (ah[i])
			return HALF_CLZ(ah[i]) + r;

	return r;
}
