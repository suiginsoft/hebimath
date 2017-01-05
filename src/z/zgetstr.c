/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstr(char *restrict str, size_t len, hebi_zsrcptr restrict a, int base)
{
	const unsigned int flags = (unsigned int)base;
	const unsigned int ubase = flags & HEBI_STR_BASEMASK;

	char *ptr;
	char *end;
	hebi_packet *w;
	size_t n;
	size_t rlen;
	int s;

	/* validate base */
	if (UNLIKELY(ubase < 2 || 36 < ubase))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* setup pointers and result length */
	ptr = str;
	end = str + len - (len > 0);
	rlen = 0;

	/* write out negative sign or optional plus sign */
	s = a->hz_sign;
	if (s < 0) {
		rlen++;
		if (LIKELY(ptr < end)) {
			*ptr = '-';
			ptr++;
			len--;
		}
	} else if (flags & HEBI_STR_SIGN) {
		rlen++;
		if (LIKELY(ptr < end)) {
			*ptr = '+';
			ptr++;
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
