/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <ctype.h>

HEBI_API
size_t
hebi_zsetstr(
		hebi_zptr restrict r,
		const char *restrict str,
		char **restrict endptr,
		int base )
{
	const char *restrict ptr, *startptr;
	unsigned char digit, digit_range, letter_range;
	int neg;

#ifdef USE_VALIDATION
	if (UNLIKELY(base && (base < 2 || 36 < base)))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
#endif

	/* skip whitespace */
	for (ptr = str; isspace(*ptr); ++ptr) ;

	/* determine sign */
	neg = 0;
	if (*ptr == '-') {
		neg = 1;
		++ptr;
	} else if (*ptr == '+') {
		++ptr;
	}

	/* determine base and skip base prefix */
	if (base == 0) {
		base = 10;
		if (ptr[0] == '0') {
			base -= 2;
			if (ptr[1] == 'x' || ptr[1] == 'X') {
				base <<= 1;
				ptr += 2;
			}
		}
	}
	else if (base == 16 && ptr[0] == '0' &&
			(ptr[1] == 'x' || ptr[1] == 'X')) {
		ptr += 2;
	}

	/* determine allowed character ranges for base */
	digit_range = (unsigned char)base;
	letter_range = 0;
	if (base > 10) {
		digit_range = 10;
		letter_range = (unsigned char)(base - 10);
	}

	/* read in the digits and accumulate result */
	hebi_zsetzero(r);
	for (startptr = ptr; ; ++ptr) {
		digit = *ptr - '0';
		if (digit >= digit_range) {
			digit = (*ptr & 0xDF) - 'A';
			if (digit >= letter_range)
				break;
			digit += 10;
		}
		hebi_zmulu(r, r, base);
		hebi_zaddu(r, r, digit);
	}

	/* finalize result */
	if (UNLIKELY(ptr <= startptr))
		ptr = str;

	if (endptr)
		*endptr = (char *)ptr;

	if (neg)
		hebi_zneg(r, r);

	return (size_t)(ptr - str);
}
