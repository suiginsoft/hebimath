/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pdivmod_norm(
		hebi_packet *restrict q,
		hebi_packet *restrict u,
		const hebi_packet *restrict v,
		size_t m,
		size_t n )
{
	HALF *restrict qh, *restrict uh;
	const HALF *restrict vh;
	FULL qhat, rhat, p;
	SFULL t, o;
	HALF f;
	size_t i, j;

	qh = HALF_PTR(q);
	uh = HALF_PTR(u);
	vh = HALF_PTR(v);

	/* clear top of last packet in quotient */
	m = m - n;
	if ((j = m % HALF_PER_PACKET) != 0)
		for (i = m; j < HALF_PER_PACKET; j++, i++)
			qh[i] = 0;

	for (j = m; j--; ) {
		/* estimate quotient and remainder */
		p = ((FULL)uh[j+n] << HALF_BITS) | uh[j+n-1];
		qhat = p / vh[n-1];
		rhat = p - qhat*vh[n-1];
	again:
		if (qhat > HALF_MAX ||
				(rhat <= HALF_MAX &&
				qhat*vh[n-2] > (rhat<<HALF_BITS)+uh[j+n-2])) {
			qhat = qhat - 1;
			rhat = rhat + vh[n-1];
			if (rhat <= HALF_MAX)
				goto again;
		}

		/* multiply and subtract */
		o = 0;
		for (i = 0; i < n; i++) {
			p = (FULL)qhat * vh[i];
			t = (SFULL)uh[i+j] - o - (SFULL)(p & HALF_MAX);
			uh[i+j] = (HALF)(t & HALF_MAX);
			o = (SFULL)(p >> HALF_BITS) - (t >> HALF_BITS);
		}
		t = uh[j+n] - o;
		uh[j+n] = (HALF)(t & HALF_MAX);

		/* finalize quotient, if we subtracted too much add it
		 * back to remainder */
		f = (HALF)(qhat & HALF_MAX);
		if (t < 0) {
			f = f - 1;
			o = 0;
			for (i = 0; i < n; i++) {
				p = uh[i+j] + vh[i] + o;
				uh[i+j] = (HALF)(p & HALF_MAX);
				o = p >> HALF_BITS;
			}
			uh[j+n] = uh[j+n] + o;
		}
		qh[j] = f;
	}

	/* compute length of quotient */
	return (m + HALF_PER_PACKET - 1) / HALF_PER_PACKET;
}
