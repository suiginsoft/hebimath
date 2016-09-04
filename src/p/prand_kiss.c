/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_prand_kiss(
		hebi_packet *restrict r,
		size_t n,
		size_t bits,
		struct hebi_kiss *restrict k )
{
	uint64_t *restrict p;
	uint64_t *restrict q;
	uint64_t m, x, c, cng, xs;
	size_t i, j, l, limbs;

	if (UNLIKELY(!n))
		return;

	/* determine number of limbs to randomly generate */
	if (UNLIKELY(!(limbs = (bits + 64 -1) / 64))) {
		hebi_pzero(r, n);
		return;
	}

	/* load PRNG state */
	xs = k->hk_xorshift;
	cng = k->hk_congruential;
	c = k->hk_carry;
	l = k->hk_length;
	if (l > 1) {
		q = k->hk_multi;
		j = k->hk_multi_index;
	} else {
		q = &k->hk_single;
		j = 0;
		l = 1;
	}

	/* generate random limbs */
	p = r->hp_limbs64;
	for (i = 0; i < limbs; i++) {
		/* multiply-with-carry generator */
		j = (j + 1) & (l - 1);
		x = q[j];
		m = (x << 28) + c;
		c = (x >> 36) - (m < x);
		m -= x;
		q[j] = m;

		/* linear congruential generator */
		cng *= UINT64_C(6906969069);
		cng += 13579;

		/* xor-shift generator */
		xs ^= xs << 13;
		xs ^= xs << 17;
		xs ^= xs << 43;

		p[i] = m + cng + xs;
	}

	/* save PRNG state */
	k->hk_xorshift = xs;
	k->hk_congruential = cng;
	k->hk_carry = c;
	if (l > 1)
		k->hk_multi_index = j;

	/* mask off bits of last limb */
	if (LIKELY(bits %= 64))
		p[i-1] &= (UINT64_C(1) << bits) - 1;

	/* zero remaining limbs of last packet */
	if (LIKELY(limbs %= HEBI_PACKET_LIMBS64))
		for ( ; limbs < HEBI_PACKET_LIMBS64; limbs++, i++)
			p[i] = 0;

	/* zero remaining packets */
	if (UNLIKELY((i /= HEBI_PACKET_LIMBS64) < n))
		hebi_pzero(r + i, n - i);
}
