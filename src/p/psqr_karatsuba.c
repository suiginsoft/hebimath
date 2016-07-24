/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_psqr_karatsuba(
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		const hebi_packet *restrict a,
		size_t n )
{
	size_t k, l, m;

	/* base case, use long squaring for at most 4 packets */
	if (n <= 4) {
		hebi_psqr(r, a, n);
		return;
	}

	/* compute length of result and partition point */
	l = n + n + 1;
	m = (n + 1) / 2;
	n -= m;

	/* compute a0+a1  */
	hebi_psetu(w+m, hebi_padd(w, a, a+m, m, n));

	/* accumulate (a0+a1)(a0+a1)*B^m */
	hebi_psqr_karatsuba(r+m, w+m+1, w, m+1);

	/* compute a1a1 */
	k = n * 2;
	hebi_pzero(w, k+1);
	hebi_psqr_karatsuba(w, w+k+1, a+m, n);

	/* accumulate a1a1*B^(m*2) - a1a1*B^m */
	hebi_padd(r+m*2, r+m*2, w, l-m*2, k);
	hebi_psub(r+m, r+m, w, l-m, k);

	/* compute a0a0 */
	k = m * 2;
	hebi_pzero(w, k+1);
	hebi_psqr_karatsuba(w, w+k+1, a, m);

	/* accumulate a0b0 - a0b0*B^m */
	hebi_padd(r, r, w, l, k);
	hebi_psub(r+m, r+m, w, l-m, k);
}
