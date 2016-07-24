/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API HEBI_CONST
size_t
hebi_psqr_karatsuba_space(size_t n)
{
	return n > 4 ? 2 * (n - 4 + 3 * hebi_floorlog2(n - 3)) : 0;
}
