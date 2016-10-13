/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <stdarg.h>

HEBI_API
void
hebi_zinits(hebi_zptr r, ...)
{
	hebi_allocid id;
	va_list ap;

	if (!r)
		return;

	id = hebi_alloc_get_default();
	va_start(ap, r);

	do {
		r->hz_packs = NULL;
		r->hz_resv = 0;
		r->hz_used = 0;
		r->hz_sign = 0;
		r->hz_allocid = (int)(intptr_t)id;
		r = va_arg(ap, hebi_zptr);
	} while(r);

	va_end(ap);
}
