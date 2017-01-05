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
	int s;

	s = a->hz_sign;
	if (UNLIKELY(!b)) {
		hebi_error_raise(HEBI_ERRDOM_HEBI,
				s ? HEBI_EDIVZERO : HEBI_EZERODIVZERO);
	}

	if (UNLIKELY(!s)) {
		if (q)
			hebi_zsetzero(q);
		return 0;
	}

	n = a->hz_used;

	if (q) {
		p = hebi_zgrow__(q, n);
		r = hebi_pdivremu(p, a->hz_packs, b, n);
		n = hebi_pnorm(p, n);
		if (n) {
			q->hz_used = n;
			q->hz_sign = s;
		} else {
			q->hz_sign = 0;
		}
	} else {
		p = hebi_pscratch__(n);
		r = hebi_pdivremu(p, a->hz_packs, b, n);
	}

	return r;
}
