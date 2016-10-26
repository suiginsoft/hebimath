/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <stdlib.h>

void
zcheckbinop(
		void (*f)(hebi_zptr, hebi_zsrcptr, hebi_zsrcptr),
		const char* opfmt,
		int flags )
{
	char *expected = NULL;
	char *operation = NULL;
	hebi_z r, a, b;
	int x;

	assert(f);
	assert(opfmt);
	assert(check_first_iter >= 0);
	assert(check_max_perm >= 0);

	hebi_zinits(r, a, b, NULL);

	check_iter = check_first_iter;
	check_max_iter = check_max_perm * check_max_perm;
	for ( ; check_iter < check_max_iter; check_iter++) {
		/* generate test inputs */
		zpermutation(check_iter, check_max_perm, 2, a, b);
		if ((flags & LHS_NONZERO) && hebi_zzero(a))
			continue;
		if ((flags & RHS_NONZERO) && hebi_zzero(b))
			continue;

		/* get expected result and operation strings */
		x = aschkprintf(&operation, opfmt, a, b);
		assert(x >= 0 && operation);
		expected = bcputs(operation);
		assert(expected);

		/* read-only inputs */
		check_pass = 0;
		zdirty(r, a, b, NULL);
		(*f)(r, a, b);
		zcheckstr(r, expected, operation);

		/* inplace operation on lhs input */
		check_pass = 1;
		hebi_zset(r, a);
		zdirty(r, b, NULL);
		(*f)(r, r, b);
		zcheckstr(r, expected, operation);

		/* inplace operation on rhs input */
		check_pass = 2;
		hebi_zset(r, b);
		zdirty(r, a, NULL);
		(*f)(r, a, r);
		zcheckstr(r, expected, operation);

		/* cleanup this iteration */
		free(expected);
		free(operation);
	}

	hebi_zdestroys(r, a, b, NULL);
}
