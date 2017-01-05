/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zdestroy(hebi_zptr r)
{
	hebi_allocid id = hebi_zallocator(r);

	if (LIKELY(id != HEBI_ALLOC_INVALID))
		hebi_pfree(id, r->hz_packs, r->hz_resv);

	r->hz_packs = NULL;
	r->hz_resv = 0;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = HEBI_ALLOC_INVALID;
}
