/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zdivrem(hebi_zptr q, hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_z qtmp;
	hebi_z rtmp;
	hebi_packet *qp;
	hebi_packet *rp;
	hebi_packet *wp;
	size_t *rpn;
	size_t qn;
	size_t rn;
	size_t wn;
	size_t an;
	size_t bn;
	int qs;
	int rs;

	rs = a->hz_sign;
	qs = b->hz_sign;

	if (UNLIKELY(!qs)) {
		hebi_error_raise(HEBI_ERRDOM_HEBI,
				rs ? HEBI_EDIVZERO : HEBI_EZERODIVZERO);
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

	qs = SIGNXOR(qs, rs) < 0 ? -1 : 1;
	qn = an - bn + 1;
	rn = bn;
	wn = an + bn + 2;
	wp = hebi_pscratch__(wn + (!q ? qn : 0));

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
