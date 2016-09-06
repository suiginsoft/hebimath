/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

STATIC_ASSERT(KARATSUBA_SQR_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API HEBI_CONST
size_t
hebi_psqr_karatsuba_space(size_t n)
{
	size_t a0, a1, k0, k1, t0, t1;

	if (n <= KARATSUBA_SQR_CUTOFF)
		return 0;

	k0 = hebi_ceillog2sz(KARATSUBA_SQR_CUTOFF - 2);
	k1 = hebi_ceillog2sz(KARATSUBA_SQR_CUTOFF - 3);
	a0 = 2 * k0 + (((size_t)1) << (k0 - 1));
	a1 = 3 * k1 + (((size_t)1) << (k1 - 1));
	t0 = 2 * (n - a0 + 2 * hebi_ceillog2sz(n - 2));
	t1 = 2 * (n - a1 + 3 * hebi_floorlog2sz(n - 3));
	return MAX(t0, t1);
}
