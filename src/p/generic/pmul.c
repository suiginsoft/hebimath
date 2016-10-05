/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
void
hebi_pmul(
		hebi_packet *restrict r,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	MLIMB *restrict rp;
	const MLIMB *ap;
	const MLIMB *bp;
	MLIMB m, o;
	DLIMB p;
	size_t i, j;

	ASSERT(an >= bn);
	ASSERT(bn > 0);

	rp = MLIMB_PTR(r);
	ap = MLIMB_PTR(a);
	bp = MLIMB_PTR(b);
	an *= MLIMB_PER_PACKET;
	bn *= MLIMB_PER_PACKET;

	/* compute first diagonal and copy into output */
	m = ap[0];
	o = 0;
	j = 0;

	do {
		p = (DLIMB)bp[j] * m + o;
		rp[j] = (MLIMB)(p & MLIMB_MAX);
		o = (MLIMB)(p >> MLIMB_BIT);
	} while (++j < bn);

	rp[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < an; i++) {
		m = ap[i];
		o = 0;
		j = 0;

		do {
			p = (DLIMB)bp[j] * m + rp[i+j] + o;
			rp[i+j] = (MLIMB)(p & MLIMB_MAX);
			o = (MLIMB)(p >> MLIMB_BIT);
		} while (++j < bn);

		rp[i+j] = o;
	}
}
