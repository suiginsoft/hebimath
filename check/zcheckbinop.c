/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"

void
zcheckbinop(
		void (*f)(hebi_zptr, hebi_zsrcptr, hebi_zsrcptr),
		const char* op,
		int flags )
{
	hebi_z r, a, b;

	hebi_zinits(r, a, b, NULL);

	if (check_max_perm <= 0)
		check_max_perm = 128;
	check_max_iter = check_max_perm * check_max_perm;

	for (check_iter = 0; check_iter < check_max_iter; check_iter++) {
		zpermutation(check_iter, check_max_perm, 2, a, b);
		if ((flags & LHS_NONZERO) && hebi_zzero(a))
			continue;
		if ((flags & RHS_NONZERO) && hebi_zzero(b))
			continue;

		/* read-only inputs */
		(*f)(r, a, b);
		zcheckbc(r, "%Z %s %Z", a, op, b);

		/* inplace operation on lhs input */
		hebi_zset(r, a);
		(*f)(a, a, b);
		zcheckbc(a, "%Z %s %Z", r, op, b);
		hebi_zset(a, r);

		/* inplace operation on rhs input */
		hebi_zset(r, b);
		(*f)(b, a, b);
		zcheckbc(b, "%Z %s %Z", a, op, r);
		hebi_zset(b, r);
	}

	hebi_zdestroys(r, a, b, NULL);
}
