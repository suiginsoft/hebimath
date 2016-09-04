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
	size_t u;
	int s;

	s = a->hz_sign;
	if (!s)
		return 0;

	u = a->hz_used;
	if (u <= 1) {
		p = a->hz_packs;
		x = (int64_t)p->hp_limbs64[0];
		if (x >= 0 && !hebi_pcmpgtui64max(p))
			return s < 0 ? -x : x;
	}

	return s < 0 ? INT64_MIN : INT64_MAX;
}
