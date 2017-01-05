/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zaddmag(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_packet *rp;
	uint64_t carry;
	size_t au, bu;

	if (UNLIKELY(hebi_zzero(a))) {
		hebi_zabs(r, b);
		return;
	} else if (UNLIKELY(hebi_zzero(b))) {
		hebi_zabs(r, a);
		return;
	}

	au = a->hz_used;
	bu = b->hz_used;
	if (au < bu || (au == bu && r == b)) {
		SWAP(hebi_zsrcptr, a, b);
		SWAP(size_t, au, bu);
	}

	if (r == a) {
		rp = hebi_zgrowcopy__(r, au + 1);
		carry = hebi_padda(rp, b->hz_packs, au, bu);
	} else {
		rp = hebi_zgrowcopyif__(r, au + 1, r == b);
		carry = hebi_padd(rp, a->hz_packs, b->hz_packs, au, bu);
	}

	if (carry)
		hebi_psetu(rp + au++, carry);

	r->hz_used = au;
	r->hz_sign = 1;
}
