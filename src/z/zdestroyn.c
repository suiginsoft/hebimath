/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zdestroyn(hebi_zptr r, size_t n)
{
	hebi_alloc_id id;

	for ( ; n; n--, r++) {
		if (LIKELY((id = hebi_zallocator(r)) != HEBI_ALLOC_INVALID))
			hebi_free(id, r->hz_packs, r->hz_resv * sizeof(hebi_packet));
		r->hz_packs = NULL;
		r->hz_resv = 0;
		r->hz_used = 0;
		r->hz_sign = 0;
		r->hz_allocid = (int)(intptr_t)HEBI_ALLOC_INVALID;
	}
}
