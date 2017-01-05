/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

STATIC_ASSERT(KARATSUBA_SQR_CUTOFF >= 4, "must be greater than or equal to 4");

HEBI_API HEBI_CONST
size_t
hebi_psqr_karatsuba_space(size_t n)
{
	size_t x;
	size_t y;

	if (n <= KARATSUBA_SQR_CUTOFF)
		return 0;

	x = hebi_ceillog2sz__(KARATSUBA_SQR_CUTOFF - 2);
	y = hebi_ceillog2sz__(KARATSUBA_SQR_CUTOFF - 3);
	x = 2 * x + (((size_t)1) << (x - 1));
	y = 3 * y + (((size_t)1) << (y - 1));
	x = 2 * (n - x + 2 * hebi_ceillog2sz__(n - 2));
	y = 2 * (n - y + 3 * hebi_floorlog2sz__(n - 3));
	return MAX(x, y);
}
