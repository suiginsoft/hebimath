/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_pmul_karatsuba(
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	size_t k, l, m, n;

	/* base case, use long multiplication for at most 4 packets */
	if (an <= 4 || bn <= 1) {
		hebi_pmul(r, a, b, an, bn);
		return;
	}

	/* compute length of result and partition point */
	l = an + bn + 1;
	m = (an + 1) / 2;
	n = bn < m ? bn : m;

	/* compute a0+a1 and b0+b1 */
	hebi_psetu(w+m, hebi_padd(w, a, a+m, m, an-m));
	hebi_psetu(w+m+n+1, hebi_padd(w+m+1, b, b+n, n, bn-n));

	/* accumulate (a0+a1)(b0+b1)*B^m */
	hebi_pmul_karatsuba(r+m, w+m+n+2, w, w+m+1, m+1, n+1);

	/* compute a1b1 */
	k = an-m + bn-n;
	hebi_pzero(w, k+1);
	hebi_pmul_karatsuba(w, w+k+1, a+m, b+n, an-m, bn-n);

	/* accumulate a1b1*B^(m+n) - a1b1*B^m */
	hebi_padd(r+m+n, r+m+n, w, l-m-n, k);
	hebi_psub(r+m, r+m, w, l-m, k);

	/* compute a0b0 */
	k = m + n;
	hebi_pzero(w, k+1);
	hebi_pmul_karatsuba(w, w+k+1, a, b, m, n);

	/* accumulate a0b0 - a0b0*B^m */
	hebi_padd(r, r, w, l, k);
	hebi_psub(r+m, r+m, w, l-m, k);
}
