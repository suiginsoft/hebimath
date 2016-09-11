/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

STATIC_ASSERT(KARATSUBA_MUL_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API HEBI_CONST
size_t
hebi_pmul_karatsuba_space(size_t an, size_t bn)
{
	size_t a0, a1, k0, k1, t0, t1;
	size_t m, s;

	if (UNLIKELY(an < bn))
		SWAP(size_t, an, bn);

	if (bn <= KARATSUBA_MUL_CUTOFF)
		return 0;

	s = 0;
	while (an > KARATSUBA_MUL_CUTOFF) {
		m = (an + 1) / 2;
		if (bn > m)
			break;
		s += an - m + bn + 1;
		an = m;
	}

	if (an <= KARATSUBA_MUL_CUTOFF)
		return s;

	k0 = hebi_ceillog2sz(KARATSUBA_MUL_CUTOFF - 2);
	k1 = hebi_ceillog2sz(KARATSUBA_MUL_CUTOFF - 3);
	a0 = 2 * k0 + (((size_t)1) << (k0 - 1));
	a1 = 3 * k1 + (((size_t)1) << (k1 - 1));
	t0 = 2 * (an - a0 + 2 * hebi_ceillog2sz(an - 2));
	t1 = 2 * (an - a1 + 3 * hebi_floorlog2sz(an - 3));
	return s + MAX(t0, t1);
}
