/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
uint64_t
hebi_zgetsu(hebi_zsrcptr a)
{
	hebi_packet *p;

	if (a->hz_sign <= 0)
		return 0;
	
	if (a->hz_used <= 1) {
		p = a->hz_packs;
		if (!p->hp_limbs64[1])
			return p->hp_limbs64[0];
	}

	return UINT64_MAX;
}
