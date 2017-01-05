/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <stdlib.h>

void
zcheckunaryop(
		void (*f)(hebi_zptr, hebi_zsrcptr),
		const char* opfmt,
		int flags )
{
	hebi_z r, a;
	char *expected = NULL;
	char *operation = NULL;
	int x;

	assert(f);
	assert(opfmt);
	assert(check_first_iter >= 0);
	assert(check_max_perm >= 0);

	hebi_zinit(r);
	hebi_zinit(a);

	check_iter = check_first_iter;
	check_max_iter = check_max_perm;
	for ( ; check_iter < check_max_iter; check_iter++) {
		/* generate test inputs */
		zpermutation(check_iter, check_max_perm, 1, a);
		if ((flags & (LHS_NONZERO | RHS_NONZERO)) && hebi_zzero(a))
			continue;

		/* get expected result and operation strings */
		x = aschkprintf(&operation, opfmt, a);
		assert(x >= 0 && operation);
		expected = bcputs(operation);
		assert(expected);

		/* read-only input */
		check_pass = 0;
		zdirty(r, a, NULL);
		(*f)(r, a);
		zcheckstr(r, expected, operation);

		/* inplace operation */
		check_pass = 1;
		hebi_zset(r, a);
		zdirty(r, NULL);
		(*f)(r, r);
		zcheckstr(r, expected, operation);

		/* cleanup this iteration */
		free(expected);
		free(operation);
	}

	hebi_zdestroy(r);
	hebi_zdestroy(a);
}
