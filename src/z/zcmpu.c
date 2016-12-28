/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

extern HEBI_API
int
hebi_zcmpu(hebi_zsrcptr a, uint64_t b)
{
	int i;

	if (UNLIKELY(!b))
		return a->hz_sign;
	else if (UNLIKELY(a->hz_sign <= 0))
		return -1;

	for (i = HEBI_PACKET_LIMBS64 - 1; i > 0; i--)
		if (a->hz_packs->hp_limbs64[i])
			return 1;

	if (a->hz_packs->hp_limbs64[0] < b)
		return -1;
	else if (a->hz_packs->hp_limbs64[0] > b)
		return 1;

	return 0;
}
