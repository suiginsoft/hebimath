/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinit_copy(hebi_zptr restrict r, hebi_zsrcptr restrict a)
{
	const struct hebi_allocfnptrs* fp;
	hebi_allocid id;
	hebi_packet *p;
	size_t n;

	fp = hebi_alloc_query(&id, HEBI_ALLOC_DEFAULT);

	p = NULL;
	n = a->hz_used;
	if (LIKELY(n)) {
		p = hebi_pallocfp(fp, n);
		hebi_pcopy(p, a->hz_packs, n);
	}

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = n;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = id;
}
