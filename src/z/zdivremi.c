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
	uint64_t bu;
	uint64_t ru;
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

	bu = (uint64_t)(b < 0 ? -b : b);
	n = a->hz_used;

	if (q) {
		p = hebi_zgrow__(q, n);
		ru = hebi_pdivremu(p, a->hz_packs, bu, n);
		n = hebi_pnorm(p, n);
		if (n) {
			q->hz_used = n;
			q->hz_sign = SIGNXOR64(s, b) < 0 ? -1 : 1;
		} else {
			q->hz_sign = 0;
		}
	} else {
		p = hebi_pscratch__(n);
		ru = hebi_pdivremu(p, a->hz_packs, bu, n);
	}

	return s < 0 ? -(int64_t)ru : (int64_t)ru;
}
