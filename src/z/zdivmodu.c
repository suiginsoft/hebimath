/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
uint64_t
hebi_zdivmodu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	hebi_packet *p;
	hebi_word m;
	size_t n;
	int e, s;

	s = a->hz_sign;
	if (UNLIKELY(!b)) {
		e = s ? HEBI_EDIVZERO : HEBI_EZERODIVZERO;
		hebi_error_raise(HEBI_ERRDOM_HEBI, e);
	}

	if (UNLIKELY(!s)) {
		if (LIKELY(r))
			hebi_zsetzero(r);
		return 0;
	}

	n = a->hz_used;
	if (n > r->hz_resv)
		hebi_zrealloczero(r, n);

	p = r->hz_packs;
	m = hebi_pdivmodu(p, a->hz_packs, b, n);
	r->hz_used = hebi_pnorm(p, n);
	r->hz_sign = r->hz_used ? s : 0;
	return m;
}
