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
	hebi_zptr restrict qz;
	hebi_zptr restrict rz;
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

	qz = q;
	if (qz) {
		if (qz == a || qz == b)
			hebi_zinit_push__((qz = qtmp), hebi_zallocator(q));
		if (qn > qz->hz_resv)
			hebi_zrealloczero(qz, qn);
		qp = qz->hz_packs;
	} else {
		qp = wp + wn;
	}

	rz = r;
	if (rz) {
		if (rz == a || rz == b)
			hebi_zinit_push__((rz = rtmp), hebi_zallocator(r));
		if (rn > rz->hz_resv)
			hebi_zrealloczero(rz, rn);
		rp = rz->hz_packs;
		rpn = &rn;
	} else {
		rp = NULL;
		rpn = NULL;
	}

	qn = hebi_pdivrem(qp, rp, wp, rpn, a->hz_packs, b->hz_packs, an, bn);

	if (rz) {
		rz->hz_used = rn;
		rz->hz_sign = rn ? rs : 0;
		if (rz != r) {
			hebi_zswap(rz, r);
			hebi_zdestroy_pop__(rz);
		}
	}

	if (qz) {
		qz->hz_used = qn;
		qz->hz_sign = qn ? qs : 0;
		if (qz != q) {
			hebi_zswap(qz, q);
			hebi_zdestroy_pop__(qz);
		}
	}
}
