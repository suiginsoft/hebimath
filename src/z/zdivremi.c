/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int64_t
hebi_zdivremi(hebi_zptr r, hebi_zsrcptr a, int64_t b)
{
	return hebi_zdivremu(r, a, (uint64_t)b);
}

