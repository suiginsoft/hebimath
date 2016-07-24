/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "internal.h"

HEBI_API
int hebi_zdivmod(hebi_zptr q, hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_z stemp, ttemp;
	hebi_allocator *sal, *tal;
	hebi_zptr restrict s;
	hebi_zptr restrict t;
	hebi_packet *restrict sp, *restrict tp, *restrict wp;
	size_t *restrict tpn;
	size_t au, bu, sn, tn, wn, wbytes;
	ptrdiff_t sign;
	int c, err;

	if (UNLIKELY(!q || !(sal = q->hz_alloc)))
		sal = hebi_default_context();

	if (UNLIKELY(q == r))
		return hebi_raise(sal, hebi_badvalue);

	bu = HEBI_ABS(b->hz_used);
	if (UNLIKELY(!bu))
		return hebi_raise(sal, hebi_divzero);

	sign = a->hz_used ^ b->hz_used;
	au = HEBI_ABS(a->hz_used);

	c = 1;
	if (au == bu)
		c = hebi_pcmp(a->hz_packs, b->hz_packs, au);
	else if (UNLIKELY(au < bu))
		c = -1;

	if (UNLIKELY(c <= 0)) {
		if (r) {
			if (c < 0) {
				err = hebi_zset(r, a);
				if (UNLIKELY(err != hebi_success))
					return err;
				err = hebi_zabs(r, r);
			} else {
				err = hebi_zsetu(r, 0);
			}
			if (UNLIKELY(err != hebi_success))
				return err;
		}
		if (q) {
			err = hebi_zsetu(q, !c ? (sign < 0 ? -1 : 1) : 0);
			if (UNLIKELY(err != hebi_success))
				return err;
		}
		return hebi_success;
	}

	s = q;
	if (q == a || q == b)
		hebi_zinit_context((s = stemp), sal);

	t = r;
	if (r == a || r == b) {
		if (!r || !(tal = r->hz_alloc))
			tal = sal;
		hebi_zinit_context((t = ttemp), tal);
	}

	sn = au - bu + 1;
	tn = bu;
	wn = au + bu + 2;

	wbytes = (wn + (!s ? sn : 0)) * HEBI_PACKET_SIZE;
	if (UNLIKELY(wbytes / HEBI_PACKET_SIZE != wn))
		return hebi_raise(sal, hebi_nomem);
	wp = hebi_temp_alloc(sal, HEBI_PACKET_ALIGNMENT, wbytes);
	if (UNLIKELY(!wp))
		return hebi_raise(sal, hebi_nomem);

	if (s) {
		if (sn > s->hz_resv) {
			err = hebi_zgrow_no_copy(s, sn);
			if (UNLIKELY(err)) {
				hebi_temp_free(sal, wp, wbytes);
				return err;
			}
		}
		sp = s->hz_packs;
	} else {
		sp = wp + wn;
	}

	if (t) {
		if (tn > t->hz_resv) {
			err = hebi_zgrow_no_copy(t, tn);
			if (UNLIKELY(err)) {
				if (s == stemp)
					hebi_zdestroy(s);
				hebi_temp_free(sal, wp, wbytes);
				return err;
			}
		}
		tp = t->hz_packs;
		tpn = (size_t *)&t->hz_used;
	} else {
		tp = NULL;
		tpn = NULL;
	}

	sn = hebi_pdivmod(sp, tp, wp, tpn, a->hz_packs, b->hz_packs, au, bu);
	hebi_temp_free(sal, wp, wbytes);

	if (q) {
		s->hz_used = sign < 0 ? -(ptrdiff_t)sn : (ptrdiff_t)sn;
		if (q != s) {
			hebi_zswap(q, s);
			hebi_zdestroy(s);
		}
	}

	if (r != t) {
		hebi_zswap(r, t);
		hebi_zdestroy(t);
	}

	return hebi_success;
}
