/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <string.h>

enum { FLAGS = HEBI_STR_RADIX | HEBI_STR_SIGN };

HEBI_API
int
hebi_zsetstr(
		hebi_zptr restrict r,
		const char *restrict str,
		char **restrict endptr,
		unsigned int base,
		unsigned int flags )
{
	hebi_z q;
	struct hebi_psetstrstate state;
	size_t len;
	size_t space;
	size_t used;

	hebi_zinit_push__(q, hebi_zallocator(r));

	len = strlen(str);
	space = hebi_psetstrprepare(&state, str, len, base, flags | FLAGS);
	if (0 < space && space != SIZE_MAX) {
		hebi_zexpand__(q, space, 0);
		used = hebi_psetstr(q->hz_packs, space, &state);
	} else {
		used = space;
	}

	if (endptr)
		*endptr = (char *)str + state.hm_cur;

	if (UNLIKELY(used == SIZE_MAX)) {
		if (state.hm_errcode != HEBI_EBADSYNTAX)
			hebi_error_raise(HEBI_ERRDOM_HEBI, state.hm_errcode);
		hebi_zsetzero(r);
		hebi_zdestroy_pop__(q);
		return 0;
	}

	ASSERT(used <= space);
	q->hz_used = used;
	q->hz_sign = state.hm_sign;
	hebi_zswap(q, r);
	hebi_zdestroy_pop__(q);
	return 1;
}
