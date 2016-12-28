/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int
hebi_zcmpi(hebi_zsrcptr a, int64_t b)
{
	uint64_t c;
	int i, s;

	s = a->hz_sign;

	if (UNLIKELY(!b))
		return s;
	else if (UNLIKELY(!s))
		return b < 0 ? 1 : -1;

	if (b < 0) {
		if (s > 0)
			return 1;
		c = (uint64_t)(-b);
	} else {
		if (s < 0)
			return -1;
		c = (uint64_t)b;
	}

	for (i = HEBI_PACKET_LIMBS64 - 1; i > 0; i--)
		if (a->hz_packs->hp_limbs64[i])
			return s;

	if (a->hz_packs->hp_limbs64[0] < c)
		return -s;
	else if (a->hz_packs->hp_limbs64[0] > c)
		return s;

	return 0;
}
