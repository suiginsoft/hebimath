/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinitv(size_t count, const hebi_zptr r[count])
{
	const hebi_allocid id = hebi_alloc_get_default();
	size_t i;

	for (i = 0; i < count; i++) {
		if (LIKELY(r[i])) {
			r[i]->hz_packs = NULL;
			r[i]->hz_resv = 0;
			r[i]->hz_used = 0;
			r[i]->hz_sign = 0;
			r[i]->hz_allocid = id;
		}
	}
}
