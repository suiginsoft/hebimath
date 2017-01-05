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

	fp = hebi_alloc_query(&id, id);

	p = NULL;
	if (LIKELY(n))
		p = hebi_pallocfp(fp, n);

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = 0;
	r->hz_sign = 0;
	r->hz_allocid = id;
}
