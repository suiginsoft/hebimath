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
	size_t n, nbytes;

	fp = hebi_alloc_query(&id, HEBI_ALLOC_DEFAULT);
	n = a->hz_used;
	p = NULL;

	if (LIKELY(n)) {
		nbytes = n * sizeof(hebi_packet);
		p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, nbytes);
		hebi_pcopy(p, a->hz_packs, n);
	}

	r->hz_packs = p;
	r->hz_resv = n;
	r->hz_used = n;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = (int)(intptr_t)id;
}
