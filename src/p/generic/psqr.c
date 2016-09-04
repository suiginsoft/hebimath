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

	/* early out if length of a is zero */
	if (UNLIKELY(!n))
		return;

	rp = MLIMB_PTR(r);
	ap = MLIMB_PTR(a);

	/* compute first diagonal and copy into output */
	m = ap[0];
	o = 0;
	for (j = 0; j < n * MLIMB_PER_PACKET; j++) {
		p = (DLIMB)ap[j] * m + o;
		rp[j] = (MLIMB)(p & MLIMB_MAX);
		o = (MLIMB)(p >> MLIMB_BIT);
	}
	rp[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < n * MLIMB_PER_PACKET; i++) {
		m = ap[i];
		o = 0;
		for (j = 0; j < n * MLIMB_PER_PACKET; j++) {
			p = (DLIMB)ap[j] * m + rp[i+j] + o;
			rp[i+j] = (MLIMB)(p & MLIMB_MAX);
			o = (MLIMB)(p >> MLIMB_BIT);
		}
		rp[i+j] = (MLIMB)(o & MLIMB_MAX);
	}
}
