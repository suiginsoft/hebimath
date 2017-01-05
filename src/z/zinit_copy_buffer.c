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
	size_t u;

	if (UNLIKELY((buffer == NULL && n) ||
			((uintptr_t)buffer & (HEBI_PACKET_ALIGNMENT - 1))))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	u = hebi_zused(a);
	if (UNLIKELY(n < u || n > HEBI_PACKET_MAXLEN))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADLENGTH);

	if (LIKELY(u))
		hebi_pcopy(buffer, a->hz_packs, u);

	r->hz_packs = buffer;
	r->hz_resv = n;
	r->hz_used = u;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = HEBI_ALLOC_INVALID;
}
