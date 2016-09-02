/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

FULL
PDIVREMRU_3x2(
		HALF *q,
		const HALF *a,
		size_t n,
		int bits,
		HALF d1,
		HALF d0,
		HALF v)
{
	FULL u;
	HALF a1, a0, u2, u1, u0;
	size_t i;

	a1 = a[n-1];
	a0 = a[n-2];

	if (bits) {
		u2 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
		u1 = a0 << bits;
		u0 = a1 >> (HALF_BITS - bits);
		if (LIKELY((u2 < d1 || (u2 == d1 && u1 < d0)) && u0 == 0)) {
			q[--n] = 0;
			a0 = a[n-2];
			u1 |= a0 >> (HALF_BITS - bits);
		} else {
			u1 = u2;
			u2 = u0;
		}
		a1 = a0;
		q[--n] = 0;
		for (i = n-1; i--; ) {
			a0 = a[i];
			u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
			a1 = a0;
			u = DIVREMRU_3x2(q+i+1, u2, u1, u0, d1, d0, v);
			u2 = (HALF)(u >> HALF_BITS);
			u1 = (HALF)(u & HALF_MAX);
		}
		u0 = a1 << bits;
		u = DIVREMRU_3x2(q, u2, u1, u0, d1, d0, v);
	} else {
		u2 = a1;
		u1 = a0;
		u0 = 0;
		if (UNLIKELY(u2 > d1 || (u2 == d1 && u1 >= d0))) {
			u2 -= d1 + (u1 < d0);
			u1 -= d0;
			u0 = 1;
		}
		q[n-1] = 0;
		q[n-2] = u0;
		u = 0;
		for (i = n - 2; i--; ) {
			u0 = a[i];
			u = DIVREMRU_3x2(q+i, u2, u1, u0, d1, d0, v);
			u2 = (HALF)(u >> HALF_BITS);
			u1 = (HALF)(u & HALF_MAX);
		}
	}

	return u >> bits;
}
