/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_copy_buffer(
		hebi_zptr restrict r,
		hebi_zsrcptr restrict a,
		void *restrict buffer,
		size_t n )
{
	size_t s;

#ifdef USE_VALIDATION
	if (UNLIKELY((!buffer && n) ||
			((uintptr_t)buffer & (HEBI_PACKET_ALIGNMENT - 1))))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
#endif

	s = a->hz_used;
#ifdef USE_VALIDATION
	if (UNLIKELY(n < s))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
#endif

	hebi_pcopy(buffer, a->hz_packs, s);
	r->hz_packs = buffer;
	r->hz_resv = n;
	r->hz_used = s;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = (int)(intptr_t)HEBI_ALLOC_INVALID;
}
