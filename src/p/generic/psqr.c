/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
void
hebi_psqr(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n)
{
	HALF *restrict rh;
	const HALF *restrict ah;
	FULL p;
	HALF m;
	hebi_word o;
	size_t i, j;

	/* early out if length of a is zero */
	if (UNLIKELY(!n))
		return;

	rh = HALF_PTR(r);
	ah = HALF_PTR(a);

	/* compute first diagonal and copy into output */
	m = ah[0];
	o = 0;
	for (j = 0; j < n * HALF_PER_PACKET; j++) {
		p = (FULL)ah[j] * m + o;
		rh[j] = (HALF)(p & HALF_MAX);
		o = (hebi_word)(p >> HALF_BITS);
	}
	rh[j] = o;

	/* compute remaining diagonals and accumulate into output */
	for (i = 1; i < n * HALF_PER_PACKET; i++) {
		m = ah[i];
		o = 0;
		for (j = 0; j < n * HALF_PER_PACKET; j++) {
			p = (FULL)ah[j] * m + rh[i+j] + o;
			rh[i+j] = (HALF)(p & HALF_MAX);
			o = (hebi_word)(p >> HALF_BITS);
		}
		rh[i+j] = (HALF)(o & HALF_MAX);
	}
}
