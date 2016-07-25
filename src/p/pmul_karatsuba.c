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
	size_t j, k, l, m;
	uint64_t c;
	
	/* ensure |a| >= |b| > ⌈|a|/2⌉ */
	if (UNLIKELY(an < bn)) {
		SWAP(const hebi_packet *, a, b);
		SWAP(size_t, an, bn);
	}

	/* base case: use long multiplication */
	if (LIKELY(bn <= KARATSUBA_MUL_CUTOFF)) {
		hebi_pmul(r, a, b, an, bn);
		return;
	}

	/* if |b| <= ⌈|a|/2⌉, split multiplication into two */
	m = (an + 1) / 2;
	if (UNLIKELY(bn <= m)) {
		hebi_pmul_karatsuba(r, w, a, b, m, bn);
		hebi_pzero(w, an-m+bn+1);
		hebi_pmul_karatsuba(w, w+an-m+bn+1, a+m, b, an-m, bn);
		hebi_padd(r+m, r+m, w, an-m+bn, an-m+bn);
		return;
	}

	/* compute a0+a1 */
	j = m;
	if (UNLIKELY(c = hebi_padd(w, a, a+m, m, an-m)))
		hebi_psetu(w+j++, c);

	/* compute b0+b1 */
	k = m;
	if (UNLIKELY(c = hebi_padd(w+j, b, b+m, m, bn-m)))
		hebi_psetu(w+j+k++, c);

	/* accumulate (a0+a1)(b0+b1)*B^m */
	hebi_pmul_karatsuba(r+m, w+j+k, w, w+j, j, k);

	/* compute length of result and partition offsets */
	j = 2 * m;
	k = an + bn - j;
	l = an + bn + 1;

	/* compute a1b1 */
	hebi_pzero(w, k+1);
	hebi_pmul_karatsuba(w, w+k+1, a+m, b+m, an-m, bn-m);

	/* accumulate a1b1*B^(2m) - a1b1*B^m */
	hebi_padd(r+j, r+j, w, l-j, k);
	hebi_psub(r+m, r+m, w, l-m, k);

	/* compute a0b0 */
	hebi_pzero(w, j+1);
	hebi_pmul_karatsuba(w, w+j+1, a, b, m, m);

	/* accumulate a0b0 - a0b0*B^m */
	hebi_padd(r, r, w, l, j);
	hebi_psub(r+m, r+m, w, l-m, j);
}
