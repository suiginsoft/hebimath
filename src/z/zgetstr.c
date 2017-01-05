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
	char *ptr, *end;
	hebi_packet *restrict w;
	size_t n, rlen;

	/* validate base */
	maskedbase = base & HEBI_STR_BASEMASK;
	if (UNLIKELY(maskedbase < 2 || 36 < maskedbase))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* setup pointers and result length */
	ptr = str;
	end = str + len - (len > 0);
	rlen = 0;

	/* write out negative sign or optional plus sign */
	if ((s = a->hz_sign) < 0) {
		rlen++;
		if (LIKELY(ptr < end)) {
			*ptr++ = '-';
			len--;
		}
	} else if (base & HEBI_STR_SIGN) {
		rlen++;
		if (LIKELY(ptr < end)) {
			*ptr++ = '+';
			len--;
		}
	}

	/* copy integer data into scratch-pad buffer if non-zero */
	n = 0;
	w = NULL;

	if (LIKELY(s)) {
		n = a->hz_used;
		w = hebi_pscratch__(n);
		hebi_pcopy(w, a->hz_packs, n);
	}

	/* determine string from packet sequence */
	return hebi_pgetstr(ptr, len, w, n, base) + rlen;
}
