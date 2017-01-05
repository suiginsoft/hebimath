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
	size_t u;

	fp = hebi_alloc_query(&id, id);

	u = hebi_zused(a);
	if (n < u)
		n = u;

	p = NULL;
	if (LIKELY(n)) {
		p = hebi_pallocfp(fp, n);
		if (LIKELY(u))
			hebi_pcopy(p, a->hz_packs, u);
	}

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = u;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = id;
}
