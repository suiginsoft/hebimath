/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
size_t
hebi_zgetstr(char *restrict str, size_t n, hebi_zsrcptr restrict a, int base)
{
	const struct hebi_alloc_callbacks *cb;
	char *restrict start, *restrict end, *restrict ptr;
	hebi_packet *restrict work;
	size_t sz, nbytes;
	char remainder;
	int s;

	/* validate base */
#ifdef USE_VALIDATION
	if (UNLIKELY(base < 2 || 36 < base))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
#endif

	/* handle case of too small an output buffer */
	if (UNLIKELY(n <= 1)) {
		if (n)
			str[0] = '\0';
		return n;
	}

	/* fast-path for zero */
	s = a->hz_sign;
	if (UNLIKELY(!s)) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	/* copy integer data into work buffer */
	sz = a->hz_used;
	nbytes = sz * sizeof(hebi_packet);
	cb = hebi_alloc_query(NULL, HEBI_ALLOC_SCRATCH);
	work = hebi_alloc_cb(cb, HEBI_PACKET_ALIGNMENT, nbytes);
	(void)hebi_pcopy(work, a->hz_packs, sz);

	/* setup pointers and write negative sign */
	ptr = str;
	end = ptr + n - 1;
	if (s < 0)
		*ptr++ = '-';

	/* write out digits */
	start = ptr;
	for ( ; ptr < end && sz > 0; ++ptr) {
		remainder = (char)hebi_pdivmodu(work, work, base, sz);
		sz = hebi_pnorm(work, sz);
		*ptr = remainder + (remainder < 10 ? '0' : 'A' - 10);
	}

	/* reverse digits for right-to-left order */
	*ptr = '\0';
	for (end = ptr - 1; start < end; ++start, --end)
		SWAP(char, *start, *end);

	/* cleanup and return result */
	hebi_free_cb(cb, work, nbytes);
	return !sz ? (size_t)(ptr - str) : n;
}
