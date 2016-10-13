/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_reserve(hebi_zptr restrict r, size_t n, hebi_allocid id)
{
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p;
	size_t nbytes;

	fp = hebi_alloc_query(&id, id);
	p = NULL;

	if (LIKELY(n)) {
		nbytes = n * sizeof(hebi_packet);
#ifdef USE_VALIDATION
		if (UNLIKELY(nbytes / sizeof(hebi_packet) != n))
			hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
#endif
		p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, nbytes);
	}

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = (int)(intptr_t)id;
}
