/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_HIDDEN
MLIMB
PDIVREMRU_2x1(
		MLIMB *q,
		const MLIMB *a,
		size_t n,
		int bits,
		MLIMB d,
		MLIMB v)
{
	MLIMB a1, a0, u1, u0;
	size_t i;

	ASSERT(n > 1);
	ASSERT(MLIMB_BIT > bits && bits >= 0);
	ASSERT((d & MLIMB_HIGH_BIT) != 0);

	a1 = a[n-1];

	if (bits) {
		u0 = a1 >> (MLIMB_BIT - bits);
		u1 = a1 << bits;
		if (!u0 && u1 < d) {
			q[--n] = 0;
			a1 = a[n-1];
			u1 |= a1 >> (MLIMB_BIT - bits);
		} else {
			u1 = u0;
		}
		for (i = n-1; i--; ) {
			a0 = a[i];
			u0 = (a1 << bits) | (a0 >> (MLIMB_BIT - bits));
			a1 = a0;
			DIVREMRU_2x1(q+i+1, &u1, u0, d, v);
		}
		u0 = a1 << bits;
		DIVREMRU_2x1(q, &u1, u0, d, v);
	} else {
		u0 = 0;
		u1 = a1;
		if (UNLIKELY(u1 >= d)) {
			u0 = 1;
			u1 -= d;
		}
		q[n-1] = u0;
		for (i = n-1; i--; ) {
			u0 = a[i];
			DIVREMRU_2x1(q+i, &u1, u0, d, v);
		}
	}

	return u1 >> bits;
}
