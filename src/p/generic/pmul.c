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

	/* early out if length of b is zero (note: an >= bn >= 0) */
	if (UNLIKELY(!bn)) {
		hebi_pzero(r, an);
		return;
	}

	rp = MLIMB_PTR(r);
	ap = MLIMB_PTR(a);
	bp = MLIMB_PTR(b);

	/* compute first diagonal and copy into output */
	m = ap[0];
	o = 0;
	for (j = 0; j < bn * MLIMB_PER_PACKET; j++) {
		p = (DLIMB)bp[j] * m + o;
		rp[j] = (MLIMB)(p & MLIMB_MAX);
		o = (MLIMB)(p >> MLIMB_BIT);
	}
	rp[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < an * MLIMB_PER_PACKET; i++) {
		m = ap[i];
		o = 0;
		for (j = 0; j < bn * MLIMB_PER_PACKET; j++) {
			p = (DLIMB)bp[j] * m + rp[i+j] + o;
			rp[i+j] = (MLIMB)(p & MLIMB_MAX);
			o = (MLIMB)(p >> MLIMB_BIT);
		}
		rp[i+j] = (MLIMB)(o & MLIMB_MAX);
	}
}
