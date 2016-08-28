/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HALF
PDIVREMRU(
		hebi_packet *r,
		const hebi_packet *a,
		size_t n,
		int bits,
		HALF d,
		HALF v)
{
	HALF *rh;
	const HALF *ah;
	FULL q;
	HALF a1, a0, q1, q0, u1, u0;
	size_t i;

	if (UNLIKELY(!n))
		return 0;

	rh = HALF_PTR(r);
	ah = HALF_PTR(a);
	i = n * HALF_PER_PACKET;
	a1 = ah[i - 1];
	u1 = a1 >> (HALF_BITS - bits);

	if (UNLIKELY(u1 >= d)) {
		rh[--i] = 1;
		u1 -= d;
	}

	while (i--) {
		a0 = ah[i];
		u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
		a1 = a0;
		q = (FULL)u1 * v;
		q0 = (HALF)(q & HALF_MAX);
		q1 = (HALF)(q >> HALF_BITS);
		q0 = q0 + u0;
		q1 = q1 + u1 + (q0 < u0) + 1;
		u1 = u0 - q1 * d;
		if (u1 > q0) {
			q1--;
			u1 += d;
		}
		if (UNLIKELY(u1 >= d)) {
			q1++;
			u1 -= d;
		}
		rh[i] = q1;
	}

	return u1 >> bits;
}
