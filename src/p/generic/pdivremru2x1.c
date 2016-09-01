/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HALF
PDIVREMRU_2x1(
		hebi_packet *q,
		const hebi_packet *a,
		size_t n,
		int bits,
		HALF d,
		HALF v)
{
	HALF *qh;
	const HALF *ah;
	HALF a1, a0, u1, u0;
	size_t i;

	n *= HALF_PER_PACKET;
	qh = HALF_PTR(q);
	ah = HALF_PTR(a);
	a1 = ah[n - 1];

	if (bits) {
		u1 = a1 << bits;
		u0 = a1 >> (HALF_BITS - bits);
		if (LIKELY(u1 < d && u0 == 0)) {
			qh[--n] = 0;
			a1 = ah[n - 1];
			u1 |= a1 >> (HALF_BITS - bits);
		} else {
			u1 = u0;
		}
		for (i = n - 1; i--; ) {
			a0 = ah[i];
			u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
			a1 = a0;
			u1 = DIVREMRU_2x1(qh + i + 1, u1, u0, d, v);
		}
		u0 = a1 << bits;
		u1 = DIVREMRU_2x1(qh, u1, u0, d, v);
	} else {
		u0 = 0;
		u1 = a1;
		if (UNLIKELY(u1 >= d)) {
			u0 = 1;
			u1 -= d;
		}
		qh[n - 1] = u0;
		for (i = n - 1; i--; ) {
			u0 = ah[i];
			u1 = DIVREMRU_2x1(qh + i, u1, u0, d, v);
		}
	}

	return u1 >> bits;
}
