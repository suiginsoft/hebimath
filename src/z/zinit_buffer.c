/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_buffer(hebi_zptr restrict r, void *restrict buffer, size_t n)
{
#ifdef USE_VALIDATION
	if (UNLIKELY((!buffer && n) ||
			((uintptr_t)buffer & (HEBI_PACKET_ALIGNMENT - 1))))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
#endif

	r->hz_packs = buffer;
	r->hz_resv = n;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = (int)(intptr_t)HEBI_ALLOC_INVALID;
}
