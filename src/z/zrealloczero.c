/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zrealloczero(hebi_zptr r, size_t n)
{
	hebi_allocid id;
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p;

	fp = hebi_alloc_query(&id, hebi_zallocator(r));
	p = LIKELY(n) ? hebi_pallocfp(fp, n) : NULL;
	hebi_pfreefp(fp, r->hz_packs, r->hz_resv);

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = (int)(intptr_t)id;
}
