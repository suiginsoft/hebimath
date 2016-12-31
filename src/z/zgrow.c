/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

/*
 * NOTE: neither of these functions modify hz_size or hz_sign as we assume
 * that a function further up the call chain will take of setting them to the
 * correct values.
 */

HEBI_HIDDEN
hebi_packet *
hebi_zgrowrealloc__(hebi_zptr r, size_t n, size_t oldn)
{
	hebi_allocid id;
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p, *oldp;

	ASSERT(oldn < n);

	fp = hebi_alloc_query(&id, hebi_zallocator(r));
	p = hebi_pallocfp(fp, n);
	oldp = r->hz_packs;
	hebi_pcopy(p, oldp, r->hz_used);
	hebi_pfreefp(fp, oldp, oldn);

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_allocid = (int)(intptr_t)id;

	return p;
}

HEBI_HIDDEN
hebi_packet *
hebi_zgrowrealloczero__(hebi_zptr r, size_t n, size_t oldn)
{
	hebi_allocid id;
	const struct hebi_allocfnptrs *fp;
	hebi_packet *p;

	ASSERT(oldn < n);

	fp = hebi_alloc_query(&id, hebi_zallocator(r));
	p = hebi_pallocfp(fp, n);
	hebi_pfreefp(fp, r->hz_packs, oldn);

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_allocid = (int)(intptr_t)id;

	return p;
}
