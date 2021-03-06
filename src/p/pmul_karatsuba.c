/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

STATIC_ASSERT(KARATSUBA_MUL_CUTOFF >= 4, "must be greater than or equal to 4");

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
	uint64_t c;
	size_t j;
	size_t k;
	size_t l;
	size_t m;

	/* ensure |a| >= |b| > ⌈|a|/2⌉ */
	if (UNLIKELY(an < bn)) {
		SWAP(const hebi_packet *, a, b);
		SWAP(size_t, an, bn);
	}

	/* base case: use long multiplication */
	if (LIKELY(bn <= KARATSUBA_MUL_CUTOFF)) {
		if (LIKELY(bn > 0))
			hebi_pmul(r, a, b, an, bn);
		return;
	}

	/* if |b| <= ⌈|a|/2⌉, split multiplication into two */
	m = (an + 1) / 2;
	if (UNLIKELY(bn <= m)) {
		hebi_pmul_karatsuba(r, w, a, b, m, bn);
		hebi_pzero(w, an-m+bn+1);
		hebi_pmul_karatsuba(w, w+an-m+bn+1, a+m, b, an-m, bn);
		(void)hebi_padda(r+m, w, an-m+bn, an-m+bn);
		return;
	}

	/* compute a0+a1 */
	j = m;
	c = hebi_padd(w, a, a+m, m, an-m);
	if (UNLIKELY(c)) {
		hebi_psetu(w+j, c);
		j++;
	}

	/* compute b0+b1 */
	k = m;
	c = hebi_padd(w+j, b, b+m, m, bn-m);
	if (UNLIKELY(c)) {
		hebi_psetu(w+j+k, c);
		k++;
	}

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
	(void)hebi_padda(r+j, w, l-j, k);
	(void)hebi_psuba(r+m, w, l-m, k);

	/* compute a0b0 */
	hebi_pzero(w, j+1);
	hebi_pmul_karatsuba(w, w+j+1, a, b, m, m);

	/* accumulate a0b0 - a0b0*B^m */
	(void)hebi_padda(r, w, l, j);
	(void)hebi_psuba(r+m, w, l-m, j);
}
