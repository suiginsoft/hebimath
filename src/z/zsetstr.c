/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <string.h>

enum { FLAGS = HEBI_STR_RADIX | HEBI_STR_SIGN | HEBI_STR_TRIM };

HEBI_API
int
hebi_zsetstr(
		hebi_zptr restrict r,
		const char *restrict str,
		char **restrict endptr,
		unsigned int base,
		unsigned int flags )
{
	struct hebi_psetstrstate state;
	size_t len;
	size_t space;
	size_t used;

	len = strlen(str);
	space = hebi_psetstrprepare(&state, str, len, base, flags | FLAGS);

	if (UNLIKELY(space == SIZE_MAX)) {
		used = SIZE_MAX;
	} else if (space > 0) {
		hebi_zgrow__(r, space);
		used = hebi_psetstr(r->hz_packs, space, &state);
	} else {
		used = 0;
	}

	if (endptr)
		*endptr = (char *)str + state.hm_cur;

	if (UNLIKELY(used == SIZE_MAX)) {
		hebi_zsetzero(r);
		if (state.hm_errcode != HEBI_EBADSYNTAX)
			hebi_error_raise(HEBI_ERRDOM_HEBI, state.hm_errcode);
		return 0;
	}

	ASSERT(used <= space);
	r->hz_used = used;
	r->hz_sign = state.hm_sign;
	return 1;
}
