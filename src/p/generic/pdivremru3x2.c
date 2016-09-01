/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

FULL
PDIVREMRU_3x2(
		hebi_packet *q,
		const hebi_packet *a,
		size_t n,
		int bits,
		HALF d1,
		HALF d0,
		HALF v)
{
	HALF *qh;
	const HALF *ah;
	HALF a1, a0, u2, u1, u0;
	FULL u;
	size_t i;

	n *= HALF_PER_PACKET;
	qh = HALF_PTR(q);
	ah = HALF_PTR(a);
	a1 = ah[n - 1];
	a0 = ah[n - 2];

	if (bits) {
		u2 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
		u1 = a0 << bits;
		u0 = a1 >> (HALF_BITS - bits);
		if (LIKELY((u2 < d1 || (u2 == d1 && u1 < d0)) && u0 == 0)) {
			qh[--n] = 0;
			a0 = ah[n - 2];
			u1 |= a0 >> (HALF_BITS - bits);
		} else {
			u1 = u2;
			u2 = u0;
		}
		a1 = a0;
		qh[--n] = 0;
		for (i = n - 1; i--; ) {
			a0 = ah[i];
			u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
			a1 = a0;
			u = DIVREMRU_3x2(qh + i + 1, u2, u1, u0, d1, d0, v);
			u2 = (HALF)(u >> HALF_BITS);
			u1 = (HALF)(u & HALF_MAX);
		}
		u0 = a1 << bits;
		u = DIVREMRU_3x2(qh, u2, u1, u0, d1, d0, v);
	} else {
		u2 = a1;
		u1 = a0;
		u0 = 0;
		if (UNLIKELY(u2 > d1 || (u2 == d1 && u1 >= d0))) {
			u2 -= d1 + (u1 < d0);
			u1 -= d0;
			u0 = 1;
		}
		qh[n - 1] = 0;
		qh[n - 2] = u0;
		for (i = n - 2; i--; ) {
			u0 = ah[i];
			u = DIVREMRU_3x2(qh + i, u2, u1, u0, d1, d0, v);
			u2 = (HALF)(u >> HALF_BITS);
			u1 = (HALF)(u & HALF_MAX);
		}
	}

	return u >> bits;
}
