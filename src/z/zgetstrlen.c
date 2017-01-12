/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstrlen(hebi_zsrcptr a, unsigned int base, unsigned int flags)
{
	const hebi_packet *p;
	size_t n;
	size_t rlen;
	int s;

	/* validate input arguments */
	if (UNLIKELY(base < 2 || 64 < base))
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

	return rlen + hebi_pgetstrlen(p, n, base, flags);
}
