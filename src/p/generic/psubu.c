/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_psubu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n)
{
	uint64_t *rl;
	const uint64_t *al;
	uint64_t diff, borrow;
	size_t i;

	ASSERT(n > 0);

#ifdef USE_LIMB32_ARITHMETIC
	if (b <= UINT32_MAX) {
		uint32_t *rl32;
		const uint32_t *al32;
		uint32_t diff32, borrow32;

		rl32 = r->hp_limbs32;
		al32 = a->hp_limbs32;
		borrow32 = (uint32_t)b;
		i = 0;

		do {
			diff32 = al32[i] - borrow32;
			borrow32 = diff32 > al32[i];
			rl32[i] = diff32;
		} while (++i < n * HEBI_PACKET_LIMBS32);

		return borrow32;
	}
#endif

	rl = r->hp_limbs64;
	al = a->hp_limbs64;
	borrow = b;
	i = 0;

	do {
		diff = al[i] - borrow;
		borrow = diff > al[i];
		rl[i] = diff;
	} while (++i < n * HEBI_PACKET_LIMBS64);

	return borrow;
}
