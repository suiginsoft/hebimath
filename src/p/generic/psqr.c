/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
void
hebi_psqr(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n)
{
	MLIMB *restrict rp;
	const MLIMB *restrict ap;
	MLIMB m, o;
	DLIMB p;
	size_t i, j;

	ASSERT(n > 0);

	rp = MLIMB_PTR(r);
	ap = MLIMB_PTR(a);
	n *= MLIMB_PER_PACKET;

	/* compute first diagonal and copy into output */
	m = ap[0];
	o = 0;
	j = 0;

	do {
		p = (DLIMB)ap[j] * m + o;
		rp[j] = (MLIMB)(p & MLIMB_MAX);
		o = (MLIMB)(p >> MLIMB_BIT);
	} while (++j < n);

	rp[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < n; i++) {
		m = ap[i];
		o = 0;
		j = 0;

		do {
			p = (DLIMB)ap[j] * m + rp[i+j] + o;
			rp[i+j] = (MLIMB)(p & MLIMB_MAX);
			o = (MLIMB)(p >> MLIMB_BIT);
		} while (++j < n);

		rp[i+j] = (MLIMB)(o & MLIMB_MAX);
	}
}
