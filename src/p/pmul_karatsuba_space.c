/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API HEBI_CONST
size_t
hebi_pmul_karatsuba_space(size_t m, size_t n)
{
	return m > 4 && n > 1 ? (2 * (m - 4 + 3 * hebi_floorlog2(m - 3))) : 0;
}
