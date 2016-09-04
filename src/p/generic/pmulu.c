/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_pmulu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n)
{
#ifdef USE_LIMB64_MULDIV

	uint64_t *rl;
	const uint64_t *al;
	hebi_uint128 p;
	uint64_t o;
	size_t i;

	rl = r->hp_limbs64;
	al = a->hp_limbs64;
	o = 0;

	for (i = 0; i < n * HEBI_PACKET_LIMBS64; ++i) {
		p = (hebi_uint128)al[i] * b + o;
		rl[i] = (uint64_t)(p & UINT64_MAX);
		o = (uint64_t)(p >> 64);
	}

	return o;

#else

	uint32_t *rl32;
	const uint32_t *al32;
	uint64_t o64;
	size_t i;

	rl32 = r->hp_limbs32;
	al32 = a->hp_limbs32;

	if (b <= UINT32_MAX) {
		const uint32_t b32 = (uint32_t)b;
		uint64_t p64;
		uint32_t o32;

		o32 = 0;
		for (i = 0; i < n * HEBI_PACKET_LIMBS32; ++i) {
			p64 = (uint64_t)al32[i] * b32 + o32;
			rl32[i] = (uint32_t)(p64 & UINT32_MAX);
			o32 = (uint32_t)(p64 >> 32);
		}
		o64 = o32;
	} else {
		const uint32_t b32_lo = (uint32_t)(b & UINT32_MAX);
		const uint32_t b32_hi = (uint32_t)(b >> 32);
		uint64_t p64_lo, p64_hi;

		o64 = 0;
		for (i = 0; i < n * HEBI_PACKET_LIMBS32; ++i) {
			p64_lo = (uint64_t)al32[i] * b32_lo + (o64 & UINT32_MAX);
			p64_hi = (uint64_t)al32[i] * b32_hi + (o64 >> 32);
			rl32[i] = (uint32_t)(p64_lo & UINT32_MAX);
			o64 = (p64_lo >> 32) + p64_hi;
		}
	}

	return o64;

#endif
}
