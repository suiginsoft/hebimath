/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_fixed(hebi_zptr restrict r, void *restrict buffer, size_t n)
{
	if (UNLIKELY((!buffer && n) ||
			((uintptr_t)buffer & (HEBI_PACKET_ALIGNMENT - 1))))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
	else if (UNLIKELY(n > HEBI_PACKET_MAXLEN))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADLENGTH);

	r->hz_packs = buffer;
	r->hz_resv = n;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = HEBI_ALLOC_FIXED;
}
