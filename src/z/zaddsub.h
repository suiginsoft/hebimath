/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

static inline HEBI_ALWAYSINLINE
void
hebi_zaddsub__(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b, int as, int bs)
{
	hebi_packet *rp;
	uint64_t carry;
	size_t au;
	size_t bu;
	int c;

	au = a->hz_used;
	bu = b->hz_used;

	if ((au < bu) || ((au == bu) && (r == b))) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, au, bu);
		SWAP(int, as, bs);
	}

	if (SIGNXOR(as, bs) >= 0) {
		if (r == a) {
			rp = hebi_zgrowcopy__(r, au + 1);
			carry = hebi_padda(rp, b->hz_packs, au, bu);
		} else {
			rp = hebi_zgrowcopyif__(r, au + 1, r == b);
			carry = hebi_padd(rp, a->hz_packs, b->hz_packs, au, bu);
		}
		if (carry != 0) {
			hebi_psetu(rp + au, carry);
			au++;
		}
	} else {
		if (au == bu) {
			c = hebi_pcmp(a->hz_packs, b->hz_packs, au);
			if (UNLIKELY(!c)) {
				hebi_zsetzero(r);
				return;
			}
			if (c < 0) {
				SWAP(hebi_zsrcptr, a, b);
				as = -as;
			}
		}
		if (r == a) {
			rp = r->hz_packs;
			(void)hebi_psuba(rp, b->hz_packs, au, bu);
		} else {
			rp = hebi_zgrowcopyif__(r, au, r == b);
			(void)hebi_psub(rp, a->hz_packs, b->hz_packs, au, bu);
		}
		au = hebi_pnorm(rp, au);
	}

	r->hz_used = au;
	r->hz_sign = as;
}
