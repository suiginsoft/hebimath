/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinitn(hebi_zptr r, size_t n)
{
	hebi_alloc_id id = hebi_alloc_get_default();

	for ( ; n; n--, r++) {
		r->hz_packs = NULL;
		r->hz_resv = 0;
		r->hz_used = 0;
		r->hz_sign = 0;
		r->hz_allocid = (int)(intptr_t)id;
	}
}
