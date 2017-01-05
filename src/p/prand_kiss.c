/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API
void
hebi_prand_kiss(
		hebi_packet *restrict r,
		size_t n,
		size_t bits,
		struct hebi_kiss *restrict k )
{
	size_t index;           /* mwc data sample index */
	size_t length;          /* mwc data sample length */
	uint64_t *data;         /* mwc data samples */
	uint64_t carry;         /* mwc carry value */
	uint64_t mwc;           /* multiply-with-carry result */
	uint64_t cng;           /* linear congruential result */
	uint64_t xs;            /* xor-shift result */
	uint64_t *limbs;
	size_t nlimbs;
	size_t i;

	ASSERT(n > 0);

	/* determine number of 64-bit limbs to randomly generate */
	nlimbs = (bits + 63) / 64;
	if (UNLIKELY(!nlimbs)) {
		hebi_pzero(r, n);
		return;
	}
	limbs = r->hp_limbs64;

	/* load PRNG state */
	xs = k->hk_xorshift;
	cng = k->hk_congruential;
	carry = k->hk_carry;
	length = k->hk_length;
	if (length > 1) {
		data = k->hk_multi;
		index = k->hk_index;
	} else {
		data = k->hk_single;
		index = 0;
		length = 1;
	}

	/* generate random limbs */
	for (i = 0; i < nlimbs; i++) {
		/* multiply-with-carry generator */
		index = (index + 1) & (length - 1);
		mwc = (data[index] << 28) + carry;
		carry = (data[index] >> 36) - (mwc < data[index]);
		mwc -= data[index];
		data[index] = mwc;

		/* linear congruential generator */
		cng *= UINT64_C(6906969069);
		cng += 13579;

		/* xor-shift generator */
		xs ^= xs << 13;
		xs ^= xs << 17;
		xs ^= xs << 43;

		limbs[i] = mwc + cng + xs;
	}

	/* save PRNG state */
	k->hk_xorshift = xs;
	k->hk_congruential = cng;
	k->hk_carry = carry;
	k->hk_index = index;

	/* mask off bits of last limb */
	if (LIKELY(bits %= 64))
		limbs[i-1] &= (UINT64_C(1) << bits) - 1;

	/* zero remaining limbs of last packet */
	STATIC_ASSERT(HEBI_PACKET_LIMBS64 == 2, "limbs-per-packet must be 2");
	if ((i % HEBI_PACKET_LIMBS64) != 0)
		limbs[i] = 0;

	/* zero remaining packets */
	i = (i + HEBI_PACKET_LIMBS64 - 1) / HEBI_PACKET_LIMBS64;
	if (i < n)
		hebi_pzero(r + i, n - i);
}
