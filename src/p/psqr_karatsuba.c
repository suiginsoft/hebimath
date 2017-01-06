/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

STATIC_ASSERT(KARATSUBA_SQR_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API
void
hebi_psqr_karatsuba(
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		const hebi_packet *restrict a,
		size_t n )
{
	uint64_t c;
	size_t j;
	size_t k;
	size_t l;
	size_t m;

	/* base case: use long multiplication if less or equal to cutoff */
	if (LIKELY(n <= KARATSUBA_SQR_CUTOFF)) {
		if (LIKELY(n > 0))
			hebi_psqr(r, a, n);
		return;
	}

	/* calculate partition point */
	m = (n + 1) / 2;

	/* compute a0+a1  */
	j = m;
	c = hebi_padd(w, a, a+m, m, n-m);
	if (UNLIKELY(c)) {
		hebi_psetu(w+j, c);
		j++;
	}

	/* accumulate (a0+a1)(a0+a1)*B^m */
	hebi_psqr_karatsuba(r+m, w+j, w, j);

	/* calculate length of result and partition offsets */
	j = 2 * (n - m);
	k = 2 * m;
	l = n + n + 1;

	/* compute a1a1 */
	hebi_pzero(w, j+1);
	hebi_psqr_karatsuba(w, w+j+1, a+m, n-m);

	/* accumulate a1a1*B^(m*2) - a1a1*B^m */
	(void)hebi_padda(r+k, w, l-k, j);
	(void)hebi_psuba(r+m, w, l-m, j);

	/* compute a0a0 */
	hebi_pzero(w, k+1);
	hebi_psqr_karatsuba(w, w+k+1, a, m);

	/* accumulate a0b0 - a0b0*B^m */
	(void)hebi_padda(r, w, l, k);
	(void)hebi_psuba(r+m, w, l-m, k);
}
