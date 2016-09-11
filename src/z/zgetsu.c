/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
uint64_t
hebi_zgetsu(hebi_zsrcptr a)
{
	if (a->hz_sign <= 0)
		return 0;
	else if (a->hz_used > 1)
		return UINT64_MAX;
	return hebi_pgetsu__(a->hz_packs);
}
