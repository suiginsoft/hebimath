/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstrlen(hebi_zsrcptr a, int base)
{
	const hebi_packet *p;
	int maskedbase, s;
	size_t n, rlen;

	/* validate base */
	maskedbase = base & HEBI_STR_BASEMASK;
	if (UNLIKELY(maskedbase < 2 || 36 < maskedbase))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* reserve space for sign */
	rlen = 0;
	if ((s = a->hz_sign) < 0 || (base & HEBI_STR_SIGN) != 0)
		rlen++;

	/* get approximated string length from packet sequence */
	n = 0;
	p = NULL;

	if (LIKELY(s)) {
		n = a->hz_used;
		p = a->hz_packs;
	}

	return hebi_pgetstrlen(p, n, base) + rlen;
}
