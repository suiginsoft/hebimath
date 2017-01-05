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
	const char *ptr;
	const char *startptr;
	unsigned int ubase;
	unsigned int digit;
	unsigned int digit_range;
	unsigned int letter_range;
	int neg;

	ubase = (unsigned int)base & HEBI_STR_BASEMASK;
	if (UNLIKELY(ubase && (ubase < 2 || 36 < ubase)))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	/* skip whitespace */
	ptr = str;
	while (isspace(*ptr))
		++ptr;

	/* determine sign */
	neg = 0;
	if (*ptr == '-') {
		neg = 1;
		++ptr;
	} else if (*ptr == '+') {
		++ptr;
	}

	/* determine base and skip base prefix */
	if (ubase == 0) {
		ubase = 10;
		if (ptr[0] == '0') {
			ubase -= 2;
			if (ptr[1] == 'x' || ptr[1] == 'X') {
				ubase <<= 1;
				ptr += 2;
			}
		}
	}
	else if (ubase == 16 && ptr[0] == '0' &&
			(ptr[1] == 'x' || ptr[1] == 'X')) {
		ptr += 2;
	}

	/* determine allowed character ranges for base */
	digit_range = ubase;
	letter_range = 0;
	if (ubase > 10) {
		digit_range = 10;
		letter_range = ubase - 10;
	}

	/* read in the digits and accumulate result */
	hebi_zsetzero(r);
	for (startptr = ptr; ; ++ptr) {
		digit = (unsigned int)*ptr - '0';
		if (digit >= digit_range) {
			digit = ((unsigned int)*ptr & 0xDF) - 'A';
			if (digit >= letter_range)
				break;
			digit += 10;
		}
		hebi_zmulu(r, r, ubase);
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
