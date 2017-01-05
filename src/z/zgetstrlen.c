/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstrlen(hebi_zsrcptr a, int base)
{
	const unsigned int flags = (unsigned int)base;
	const unsigned int ubase = flags & HEBI_STR_BASEMASK;

	const hebi_packet *p;
	size_t n;
	size_t rlen;
	int s;

	/* validate base */
	if (UNLIKELY(ubase < 2 || 36 < ubase))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* reserve space for sign */
	rlen = 0;
	if ((s = a->hz_sign) < 0 || (flags & HEBI_STR_SIGN) != 0)
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
