/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

STATIC_ASSERT(KARATSUBA_MUL_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API HEBI_CONST
size_t
hebi_pmul_karatsuba_space(size_t m, size_t n)
{
	size_t a = m;
	size_t b = n;
	size_t mid;
	size_t min;
	size_t x;
	size_t y;

	if (UNLIKELY(a < b))
		SWAP(size_t, a, b);

	if (b <= KARATSUBA_MUL_CUTOFF)
		return 0;

	min = 0;
	while (a > KARATSUBA_MUL_CUTOFF) {
		mid = (a + 1) / 2;
		if (b > mid)
			break;
		min += a - mid + b + 1;
		a = mid;
	}

	if (a <= KARATSUBA_MUL_CUTOFF)
		return min;

	x = hebi_ceillog2sz__(KARATSUBA_MUL_CUTOFF - 2);
	y = hebi_ceillog2sz__(KARATSUBA_MUL_CUTOFF - 3);
	x = 2 * x + (((size_t)1) << (x - 1));
	y = 3 * y + (((size_t)1) << (y - 1));
	x = 2 * (a - x + 2 * hebi_ceillog2sz__(a - 2));
	y = 2 * (a - y + 3 * hebi_floorlog2sz__(a - 3));
	return min + MAX(x, y);
}
