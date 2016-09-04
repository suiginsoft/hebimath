/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
PDIVREMR_3x2(
		MLIMB *restrict q,
		MLIMB *restrict u,
		const MLIMB *restrict d,
		MLIMB v,
		size_t m,
		size_t n )
{
	DLIMB qhat, rhat, p;
	DLIMB t, o;
	MLIMB f;
	size_t i, j;

	/* clear top of last packet in quotient */
	m = m - n;
	if ((j = m % MLIMB_PER_PACKET) != 0)
		for (i = m; j < MLIMB_PER_PACKET; j++, i++)
			q[i] = 0;

	for (j = m; j--; ) {
		/* estimate quotient and remainder */
		p = ((DLIMB)u[j+n] << MLIMB_BIT) | u[j+n-1];
		qhat = p / d[n-1];
		rhat = p - qhat*d[n-1];
	again:
		if (qhat > MLIMB_MAX ||
				(rhat <= MLIMB_MAX &&
				qhat*d[n-2] > (rhat<<MLIMB_BIT)+u[j+n-2])) {
			qhat = qhat - 1;
			rhat = rhat + d[n-1];
			if (rhat <= MLIMB_MAX)
				goto again;
		}

		/* multiply and subtract */
		o = 0;
		for (i = 0; i < n; i++) {
			p = (DLIMB)qhat * d[i];
			t = (DLIMB)u[i+j] - o - (DLIMB)(p & MLIMB_MAX);
			u[i+j] = (MLIMB)(t & MLIMB_MAX);
			o = (DLIMB)(p >> MLIMB_BIT) - (t >> MLIMB_BIT);
		}
		t = u[j+n] - o;
		u[j+n] = (MLIMB)(t & MLIMB_MAX);

		/* finalize quotient, if we subtracted too much add it
		 * back to remainder */
		f = (MLIMB)(qhat & MLIMB_MAX);
		if (t < 0) {
			f = f - 1;
			o = 0;
			for (i = 0; i < n; i++) {
				p = u[i+j] + d[i] + o;
				u[i+j] = (MLIMB)(p & MLIMB_MAX);
				o = p >> MLIMB_BIT;
			}
			u[j+n] = u[j+n] + o;
		}
		q[j] = f;
	}

	/* compute length of quotient */
	return (m + MLIMB_PER_PACKET - 1) / MLIMB_PER_PACKET;
}
