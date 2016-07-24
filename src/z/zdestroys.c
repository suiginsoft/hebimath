/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <stdarg.h>

HEBI_API
void
hebi_zdestroys(hebi_zptr r, ...)
{
	hebi_alloc_id id;
	va_list ap;

	if (!r)
		return;

	va_start(ap, r);

	do {
		if (LIKELY((id = hebi_zallocator(r)) != HEBI_ALLOC_INVALID))
			hebi_free(id, r->hz_packs, r->hz_resv * sizeof(hebi_packet));
		r->hz_packs = NULL;
		r->hz_resv = 0;
		r->hz_used = 0;
		r->hz_sign = 0;
		r->hz_allocid = (int)(intptr_t)HEBI_ALLOC_INVALID;
		r = va_arg(ap, hebi_zptr);
	} while(r);

	va_end(ap);
}
