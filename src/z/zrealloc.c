/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zrealloc(hebi_zptr r, size_t n)
{
	hebi_allocid id;
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p, *oldp;
	size_t u;

	fp = hebi_alloc_query(&id, hebi_zallocator(r));
	p = LIKELY(n) ? hebi_pallocfp(fp, n) : NULL;
	oldp = r->hz_packs;

	if ((u = MIN(n, hebi_zused(r))))
		hebi_pcopy(p, oldp, u);

	hebi_pfreefp(fp, oldp, r->hz_resv);

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = u;
	r->hz_sign = !!u;
	r->hz_allocid = (int)(intptr_t)id;
}
