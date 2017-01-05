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
	hebi_packet *rp;
	size_t rn;
	size_t ru;

	fp = hebi_alloc_query(&id, id);

	rn = n;
	ru = hebi_zused(a);
	if (rn < ru)
		rn = ru;

	rp = NULL;
	if (LIKELY(rn)) {
		rp = hebi_pallocfp(fp, rn);
		if (LIKELY(ru))
			hebi_pcopy(rp, a->hz_packs, ru);
	}

	r->hz_packs = rp;
	r->hz_resv = rn;
	r->hz_used = ru;
	r->hz_sign = a->hz_sign;
	r->hz_allocid = id;
}
