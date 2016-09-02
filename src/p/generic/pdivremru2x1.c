/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HALF
PDIVREMRU_2x1(
		HALF *q,
		const HALF *a,
		size_t n,
		int bits,
		HALF d,
		HALF v)
{
	HALF a1, a0, u1, u0;
	size_t i;

	a1 = a[n-1];

	if (bits) {
		u1 = a1 << bits;
		u0 = a1 >> (HALF_BITS - bits);
		if (LIKELY(u1 < d && u0 == 0)) {
			q[--n] = 0;
			a1 = a[n-1];
			u1 |= a1 >> (HALF_BITS - bits);
		} else {
			u1 = u0;
		}
		for (i = n-1; i--; ) {
			a0 = a[i];
			u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
			a1 = a0;
			u1 = DIVREMRU_2x1(q+i+1, u1, u0, d, v);
		}
		u0 = a1 << bits;
		u1 = DIVREMRU_2x1(q, u1, u0, d, v);
	} else {
		u0 = 0;
		u1 = a1;
		if (UNLIKELY(u1 >= d)) {
			u0 = 1;
			u1 -= d;
		}
		q[n - 1] = u0;
		for (i = n - 1; i--; ) {
			u0 = a[i];
			u1 = DIVREMRU_2x1(q+i, u1, u0, d, v);
		}
	}

	return u1 >> bits;
}
