/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_paddu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n)
{
	uint64_t *rl;
	const uint64_t *al;
	uint64_t sum, carry;
	size_t i;

#ifdef USE_LIMB32_ARITHMETIC
	if (b <= UINT32_MAX) {
		uint32_t *rl32;
		const uint32_t *al32;
		uint32_t sum32, carry32;

		rl32 = r->hp_limbs32;
		al32 = a->hp_limbs32;
		carry32 = (uint32_t)b;

		for (i = 0; i < n * HEBI_PACKET_LIMBS32; ++i) {
			sum32 = al32[i] + carry32;
			carry32 = sum32 < al32[i];
			rl32[i] = sum32;
		}

		return carry32;
	}
#endif

	rl = r->hp_limbs64;
	al = a->hp_limbs64;
	carry = b;

	for (i = 0; i < n * HEBI_PACKET_LIMBS64; ++i) {
		sum = al[i] + carry;
		carry = sum < al[i];
		rl[i] = sum;
	}

	return carry;
}
