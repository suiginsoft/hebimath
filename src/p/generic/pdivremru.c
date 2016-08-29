/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

static inline HEBI_ALWAYSINLINE
HALF
divremru(HALF *qh, HALF u1, HALF u0, HALF d, HALF v)
{
	HALF q1, q0;
	FULL q, u;

	u = ((FULL)u1 << HALF_BITS) | u0;
	q = (FULL)u1 * v + u;
	q0 = (HALF)(q & HALF_MAX);
	q1 = (HALF)(q >> HALF_BITS) + 1;
	u1 = u0 - q1 * d;

	if (u1 > q0) {
		q1--;
		u1 += d;
	}

	if (UNLIKELY(u1 >= d)) {
		q1++;
		u1 -= d;
	}

	*qh = q1;
	return u1;
}

HALF
PDIVREMRU(
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
		u1 = 0;
		if (a1 < (d >> bits)) {
			qh[--n] = u1;
			u1 = a1 << bits;
		}
		a1 = ah[n - 1];
		u1 |= a1 >> (HALF_BITS - bits);
		for (i = n - 1; i--; ) {
			a0 = ah[i];
			u0 = (a1 << bits) | (a0 >> (HALF_BITS - bits));
			a1 = a0;
			u1 = divremru(qh + i + 1, u1, u0, d, v);
		}
		u0 = a1 << bits;
		u1 = divremru(qh, u1, u0, d, v);
	} else {
		u0 = 0;
		u1 = a1;
		if (UNLIKELY(u1 >= d)) {
			u0 = 1;
			u1 -= d;
		}
		qh[--n] = u0;
		for (i = n; i--; ) {
			u0 = ah[i];
			u1 = divremru(qh + i, u1, u0, d, v);
		}
	}

	return u1 >> bits;
}
