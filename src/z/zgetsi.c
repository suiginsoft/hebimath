/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int64_t
hebi_zgetsi(hebi_zsrcptr a)
{
	const hebi_packet *p;
	int64_t x;
	int s;

	if (!(s = a->hz_sign))
		return 0;

	if (a->hz_used <= 1) {
		p = a->hz_packs;
		if (!p->hp_limbs64[1]) {
			x = (int64_t)p->hp_limbs64[0];
			if (x >= 0)
				return s < 0 ? -x : x;
		}
	}

	return s < 0 ? INT64_MIN : INT64_MAX;
}
