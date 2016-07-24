/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zrealloczero(hebi_zptr r, size_t n)
{
	hebi_alloc_id id;
	const struct hebi_alloc_callbacks *cb;
	hebi_packet *p;
	size_t nbytes;

	nbytes = n * sizeof(hebi_packet);
#ifdef USE_VALIDATION
	if (UNLIKELY(nbytes / sizeof(hebi_packet) != n))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
#endif

	cb = hebi_alloc_query(&id, hebi_zallocator(r));

	p = NULL;
	if (LIKELY(nbytes))
		p = hebi_alloc_cb(cb, HEBI_PACKET_ALIGNMENT, nbytes);

	hebi_free_cb(cb, r->hz_packs, r->hz_resv * sizeof(hebi_packet));

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_sign = 0;
	r->hz_allocid = (int)(intptr_t)id;
}
