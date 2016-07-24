/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pctz(const hebi_packet *a, size_t n)
{
	const HALF *ah;
	size_t i, r;

	if (UNLIKELY(!n))
		return 0;

	ah = HALF_PTR(a);
	for (r = 0, i = 0; i < n * HALF_PER_PACKET; r += HALF_BITS)
		if (ah[i])
			return HALF_CTZ(ah[i]) + r;

	return r;
}
