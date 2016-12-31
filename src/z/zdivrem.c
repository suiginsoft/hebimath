/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zdivrem(hebi_zptr q, hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_z qtmp, rtmp;
	hebi_packet *restrict qp, *restrict rp, *restrict wp;
	size_t qn, rn, wn, an, bn, *restrict rpn;
	int e, qs, rs;

	qs = b->hz_sign;
	rs = a->hz_sign;

	if (UNLIKELY(!qs)) {
		e = rs ? HEBI_EDIVZERO : HEBI_EZERODIVZERO;
		hebi_error_raise(HEBI_ERRDOM_HEBI, e);
	} else if (UNLIKELY(!rs)) {
		if (r)
			hebi_zsetzero(r);
		if (q)
			hebi_zsetzero(q);
		return;
	}

	an = a->hz_used;
	bn = b->hz_used;

	if (UNLIKELY(an < bn)) {
		if (r)
			hebi_zset(r, a);
		if (q)
			hebi_zsetzero(q);
		return;
	}

	qs = (qs ^ rs) < 0 ? -1 : 1;
	qn = an - bn + 1;
	rn = bn;
	wn = an + bn + 2;
	wp = hebi_pscratch__(wn + (q ? 0 : qn));

	if (q) {
		hebi_zinit_push__(qtmp, hebi_zallocator(q));
		qp = hebi_zgrow__(qtmp, qn);
	} else {
		qp = wp + wn;
	}

	if (r) {
		hebi_zinit_push__(rtmp, hebi_zallocator(r));
		rp = hebi_zgrow__(rtmp, rn);
		rpn = &rn;
	} else {
		rp = NULL;
		rpn = NULL;
	}

	qn = hebi_pdivrem(qp, rp, wp, rpn, a->hz_packs, b->hz_packs, an, bn);

	if (r) {
		hebi_zswap(rtmp, r);
		hebi_zdestroy_pop__(rtmp);
		r->hz_used = rn;
		r->hz_sign = rn ? rs : 0;
	}

	if (q) {
		hebi_zswap(qtmp, q);
		hebi_zdestroy_pop__(qtmp);
		q->hz_used = qn;
		q->hz_sign = qn ? qs : 0;
	}
}
