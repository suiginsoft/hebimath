/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_copy_reserve(
		hebi_zptr restrict r,
		hebi_zsrcptr restrict a,
		size_t n,
		hebi_allocid id )
{
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p;
	size_t nbytes, u;

	fp = hebi_alloc_query(&id, id);
	p = NULL;

	u = hebi_zused(a);
	if (n < u)
		n = u;

	if (LIKELY(n)) {
		nbytes = n * sizeof(hebi_packet);
#ifdef USE_VALIDATION
		if (UNLIKELY(nbytes / sizeof(hebi_packet) != n))
			hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
#endif
		p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, nbytes);
		if (LIKELY(u))
			hebi_pcopy(p, a->hz_packs, u);
	}

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = u;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = (int)(intptr_t)id;
}
