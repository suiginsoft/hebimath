/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zsqr(hebi_zptr r, hebi_zsrcptr a)
{
	hebi_z temp;
	hebi_zptr restrict t;
	hebi_packet *restrict tp, *restrict wp;
	size_t au, tn;
	int as;

	if (UNLIKELY(!(as = a->hz_sign))) {
		hebi_zsetzero(r);
		return;
	}

	au = a->hz_used;
	tn = au * 2 + 1;
	if (UNLIKELY(tn <= au))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);

	t = r;
	if (t == a)
		hebi_zinit_allocator((t = temp), hebi_zallocator(r));

	if (au > 4) {
		wp = hebi_pscratch(hebi_psqr_karatsuba_space(au));
		if (tn > t->hz_resv)
			hebi_zrealloczero(t, tn);
		tp = t->hz_packs;
		hebi_pzero(tp, tn);
		hebi_psqr_karatsuba(tp, wp, a->hz_packs, au);
	} else {
		if (--tn > t->hz_resv)
			hebi_zrealloczero(t, tn);
		tp = t->hz_packs;
		hebi_psqr(tp, a->hz_packs, au);
	}

	t->hz_used = hebi_pnorm(tp, tn);
	t->hz_sign = 1;

	if (r != t) {
		hebi_zswap(r, t);
		hebi_zdestroy(t);
	}
}
