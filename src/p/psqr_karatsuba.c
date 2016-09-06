/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

STATIC_ASSERT(KARATSUBA_SQR_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API
void
hebi_psqr_karatsuba(
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		const hebi_packet *restrict a,
		size_t n )
{
	size_t j, k, l, m;
	uint64_t c;

	/* base case: use long multiplication if less or equal to cutoff */
	if (LIKELY(n <= KARATSUBA_SQR_CUTOFF)) {
		hebi_psqr(r, a, n);
		return;
	}

	/* calculate partition point */
	m = (n + 1) / 2;

	/* compute a0+a1  */
	j = m;
	if (UNLIKELY(c = hebi_padd(w, a, a+m, m, n-m)))
		hebi_psetu(w+j++, c);

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
	hebi_padd(r+k, r+k, w, l-k, j);
	hebi_psub(r+m, r+m, w, l-m, j);

	/* compute a0a0 */
	hebi_pzero(w, k+1);
	hebi_psqr_karatsuba(w, w+k+1, a, m);

	/* accumulate a0b0 - a0b0*B^m */
	hebi_padd(r, r, w, l, k);
	hebi_psub(r+m, r+m, w, l-m, k);
}
