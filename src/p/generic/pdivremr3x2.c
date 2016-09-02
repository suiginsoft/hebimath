/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
PDIVREMR_3x2(
		HALF *restrict q,
		HALF *restrict u,
		const HALF *restrict d,
		HALF v,
		size_t m,
		size_t n )
{
	FULL qhat, rhat, p;
	SFULL t, o;
	HALF f;
	size_t i, j;

	/* clear top of last packet in quotient */
	m = m - n;
	if ((j = m % HALF_PER_PACKET) != 0)
		for (i = m; j < HALF_PER_PACKET; j++, i++)
			q[i] = 0;

	for (j = m; j--; ) {
		/* estimate quotient and remainder */
		p = ((FULL)u[j+n] << HALF_BITS) | u[j+n-1];
		qhat = p / d[n-1];
		rhat = p - qhat*d[n-1];
	again:
		if (qhat > HALF_MAX ||
				(rhat <= HALF_MAX &&
				qhat*d[n-2] > (rhat<<HALF_BITS)+u[j+n-2])) {
			qhat = qhat - 1;
			rhat = rhat + d[n-1];
			if (rhat <= HALF_MAX)
				goto again;
		}

		/* multiply and subtract */
		o = 0;
		for (i = 0; i < n; i++) {
			p = (FULL)qhat * d[i];
			t = (SFULL)u[i+j] - o - (SFULL)(p & HALF_MAX);
			u[i+j] = (HALF)(t & HALF_MAX);
			o = (SFULL)(p >> HALF_BITS) - (t >> HALF_BITS);
		}
		t = u[j+n] - o;
		u[j+n] = (HALF)(t & HALF_MAX);

		/* finalize quotient, if we subtracted too much add it
		 * back to remainder */
		f = (HALF)(qhat & HALF_MAX);
		if (t < 0) {
			f = f - 1;
			o = 0;
			for (i = 0; i < n; i++) {
				p = u[i+j] + d[i] + o;
				u[i+j] = (HALF)(p & HALF_MAX);
				o = p >> HALF_BITS;
			}
			u[j+n] = u[j+n] + o;
		}
		q[j] = f;
	}

	/* compute length of quotient */
	return (m + HALF_PER_PACKET - 1) / HALF_PER_PACKET;
}
