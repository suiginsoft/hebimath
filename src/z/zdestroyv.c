/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zdestroyv(size_t count, const hebi_zptr r[count])
{
	hebi_allocid id;
	size_t i;

	for (i = 0; i < count; i++) {
		if (LIKELY(r[i])) {
			id = hebi_zallocator(r[i]);
			if (LIKELY(id != HEBI_ALLOC_INVALID))
				hebi_pfree(id, r[i]->hz_packs, r[i]->hz_resv);
			r[i]->hz_packs = NULL;
			r[i]->hz_resv = 0;
			r[i]->hz_used = 0;
			r[i]->hz_sign = 0;
			r[i]->hz_allocid = HEBI_ALLOC_INVALID;
		}
	}
}
