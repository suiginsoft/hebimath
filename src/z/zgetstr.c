/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstr(char *restrict str, size_t len, hebi_zsrcptr restrict a, int base)
{
	int maskedbase, s;
	char *start, *end;
	hebi_packet *restrict w;
	size_t n, rlen;

	/* validate base */
	maskedbase = base & HEBI_STR_BASEMASK;
	if (UNLIKELY(maskedbase < 2 || 36 < maskedbase))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* setup pointers and result length */
	start = str;
	end = str + len - (len > 0);
	rlen = 0;

	/* write out negative sign or optional plus sign */
	s = a->hz_sign;
	if (s < 0) {
		if (LIKELY(str < end))
			*str++ = '-';
		rlen++;
	} else if (base & HEBI_STR_SIGN) {
		if (LIKELY(str < end))
			*str++ = '+';
		rlen++;
	}

	/* fast-path for zero */
	if (UNLIKELY(!s)) {
		if (LIKELY(str < end))
			*str++ = '0';
		if (LIKELY(len > 0))
			*str = '\0';
		return ++rlen;
	}

	/* copy integer data into scratch-pad buffer */
	n = a->hz_used;
	w = hebi_pscratch__(n);
	hebi_pcopy(w, a->hz_packs, n);

	/* get character string from packet sequence */
	len -= (size_t)(str - start);
	rlen += hebi_pgetstr(str, len, w, n, base);
	return rlen;
}
