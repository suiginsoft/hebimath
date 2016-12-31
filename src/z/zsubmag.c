/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zsubmag(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	size_t au, bu;
	int d, s;

	if (UNLIKELY(hebi_zzero(a))) {
		hebi_zabs(r, b);
		hebi_zneg(r, r);
		return;
	} else if (UNLIKELY(hebi_zzero(b))) {
		hebi_zabs(r, a);
		return;
	}

	au = a->hz_used;
	bu = b->hz_used;
	s = 1;

	if (au < bu) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, au, bu);
		s = -s;
	}

	d = 1;
	if (au == bu) {
		d = hebi_pcmp(a->hz_packs, b->hz_packs, au);
		if (d <= 0) {
			if (UNLIKELY(!d)) {
				hebi_zsetzero(r);
				return;
			}
			SWAP(hebi_zsrcptr, a, b);
		}
	}

	if (r == a) {
		rp = r->hz_packs;
		(void)hebi_psuba(rp, b->hz_packs, au, bu);
	} else {
		rp = hebi_zgrowcopyif__(r, au, r == b);
		(void)hebi_psub(rp, a->hz_packs, b->hz_packs, au, bu);
	}

	r->hz_used = hebi_pnorm(rp, au);
	r->hz_sign = d * s;
}
