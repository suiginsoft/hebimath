/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int64_t
hebi_zdivremi(hebi_zptr q, hebi_zsrcptr a, int64_t b)
{
	hebi_packet *p;
	uint64_t bu, ru;
	int64_t r;
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

	bu = (uint64_t)(b < 0 ? -b : b);
	n = a->hz_used;

	if (q) {
		if (n > q->hz_resv)
			hebi_zrealloczero(q, n);
		p = q->hz_packs;
		ru = hebi_pdivremu(p, a->hz_packs, bu, n);
		q->hz_used = hebi_pnorm(p, n);
		q->hz_sign = q->hz_used ? ((((int64_t)s ^ b) >= 0) ? 1 : -1) : 0;
	} else {
		p = hebi_pscratch(n);
		ru = hebi_pdivremu(p, a->hz_packs, bu, n);
	}

	r = s < 0 ? -(int64_t)ru : (int64_t)ru;
	return r;
}
