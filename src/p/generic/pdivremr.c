/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
PDIVREMR(
		hebi_packet *restrict q,
		MLIMB *restrict ul,
		const MLIMB *restrict dl,
		MLIMB v,
		size_t m,
		size_t n )
{
	MLIMB *restrict ql;
	MLIMB qhat, u1, u0, p1, p0;
	DLIMB p;
	size_t i, j;

	ASSERT(m >= n);
	ASSERT(n > 2);
	ASSERT((dl[n-1] & MLIMB_HIGH_BIT) != 0);

	/*
	 * ensure initial top two limbs of dividend are less than top two
	 * words of divisor, adjust dividend and quotient if necessary
	 */
	j = m -= n;
	u1 = ul[j+n-1];
	u0 = ul[j+n-2];
	ql = MLIMB_PTR(q);

	if (UNLIKELY(u1 > dl[n-1] || (u1 == dl[n-1] && u0 >= dl[n-2]))) {
		u1 -= dl[n-1] + (u0 < dl[n-2]);
		u0 -= dl[n-2];
		ul[j+n-1] = u1;
		ul[j+n-2] = u0;
		ql[m++] = 1;
	}

	/* clear top of last packet in quotient */
	if ((i = m % MLIMB_PER_PACKET) != 0)
		for ( ; i < MLIMB_PER_PACKET; i++)
			ql[m++] = 0;

	/* perform multi-word 3x2 reciprocal division */
	goto start;

	do {
		/* estimate quotient limb */
		u1 = ul[j+n-1];
		u0 = ul[j+n-2];
	start:
		DIVREMRU_3x2(&qhat, &u1, &u0, ul[j+n-3], dl[n-1], dl[n-2], v);

		/* multiply and subtract to compute remainder */
		u1 = 0;
		for (i = 0; i < n - 2; i++) {
			p = (DLIMB)qhat * dl[i];
			p0 = (MLIMB)(p & MLIMB_MAX);
			p1 = (MLIMB)(p >> MLIMB_BIT);
			p0 = p0 + u1;
			u1 = p1 + (p0 < u1);
			u0 = ul[j+i];
			p0 = u0 - p0;
			u1 = u1 + (u0 < p0);
			ul[j+i] = p0;
		}
		u0 = ul[j+n-1];
		p0 = u0 - u1;
		u1 = u0 < p0;
		ul[j+n-1] = p0;

		/* adjust if quotient estimate was too large */
		if (UNLIKELY(u1 > 0)) {
			u1 = 0;
			for (i = 0; i < n; i++) {
				u0 = ul[j+i] + dl[i] + u1;
				u1 = (u0 < ul[j+i]) || (u0 == ul[j+i] && u1);
				ul[j+i] = u0;
			}
			qhat--;
		}

		ql[j-1] = qhat;
	}
	while (--j);

	/* length of quotient in packets*/
	return m / MLIMB_PER_PACKET;
}
