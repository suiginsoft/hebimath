/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zmul(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_z temp;
	hebi_zptr restrict t;
	hebi_packet *restrict tp, *restrict wp;
	size_t au, bu, tn;
	int as, bs;

	if (UNLIKELY(!(as = a->hz_sign) || !(bs = b->hz_sign))) {
		hebi_zsetzero(r);
		return;
	}

	au = a->hz_used;
	bu = b->hz_used;
	if (au < bu) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, au, bu);
	}

	tn = au + bu + 1;
	if (UNLIKELY(tn <= au))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);

	t = r;
	if (t == a || t == b)
		hebi_zinit_allocator((t = temp), hebi_zallocator(r));

	if (au > 4) {
		wp = hebi_pscratch(hebi_pmul_karatsuba_space(au, bu));
		if (tn > t->hz_resv)
			hebi_zrealloczero(t, tn);
		tp = t->hz_packs;
		hebi_pzero(tp, tn);
		hebi_pmul_karatsuba(tp, wp, a->hz_packs, b->hz_packs, au, bu);
	} else {
		if (--tn > t->hz_resv)
			hebi_zrealloczero(t, tn);
		tp = t->hz_packs;
		hebi_pmul(tp, a->hz_packs, b->hz_packs, au, bu);
	}

	t->hz_used = hebi_pnorm(tp, tn);
	t->hz_sign = (as ^ bs) < 0 ? -1 : 1;

	if (r != t) {
		hebi_zswap(r, t);
		hebi_zdestroy(t);
	}
}
