/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
uint64_t
hebi_zdivremu(hebi_zptr q, hebi_zsrcptr a, uint64_t b)
{
	hebi_packet *p;
	uint64_t r;
	size_t n;
	int e, s;

	s = a->hz_sign;
	if (UNLIKELY(!b)) {
		e = s ? HEBI_EDIVZERO : HEBI_EZERODIVZERO;
		hebi_error_raise(HEBI_ERRDOM_HEBI, e);
	}

	if (UNLIKELY(!s)) {
		if (q)
			hebi_zsetzero(q);
		return 0;
	}

	n = a->hz_used;

	if (q) {
		if (n > q->hz_resv)
			hebi_zrealloczero(q, n);
		p = q->hz_packs;
		r = hebi_pdivremu(p, a->hz_packs, b, n);
		q->hz_used = hebi_pnorm(p, n);
		q->hz_sign = q->hz_used ? s : 0;
	} else {
		p = hebi_pscratch__(n);
		r = hebi_pdivremu(p, a->hz_packs, b, n);
	}

	return r;
}
