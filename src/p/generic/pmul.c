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
	HALF *restrict rh;
	const HALF *ah;
	const HALF *bh;
	FULL p;
	HALF m;
	hebi_word o;
	size_t i, j;

	/* early out if length of b is zero (note: an >= bn >= 0) */
	if (UNLIKELY(!bn)) {
		hebi_pzero(r, an);
		return;
	}

	rh = HALF_PTR(r);
	ah = HALF_PTR(a);
	bh = HALF_PTR(b);

	/* compute first diagonal and copy into output */
	m = ah[0];
	o = 0;
	for (j = 0; j < bn * HALF_PER_PACKET; j++) {
		p = (FULL)bh[j] * m + o;
		rh[j] = (HALF)(p & HALF_MAX);
		o = (hebi_word)(p >> HALF_BITS);
	}
	rh[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < an * HALF_PER_PACKET; i++) {
		m = ah[i];
		o = 0;
		for (j = 0; j < bn * HALF_PER_PACKET; j++) {
			p = (FULL)bh[j] * m + rh[i+j] + o;
			rh[i+j] = (HALF)(p & HALF_MAX);
			o = (hebi_word)(p >> HALF_BITS);
		}
		rh[i+j] = (HALF)(o & HALF_MAX);
	}
}
