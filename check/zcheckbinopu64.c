/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <stdlib.h>

void
zcheckbinopu64(
		void (*f)(hebi_zptr, hebi_zsrcptr, uint64_t),
		const char* opfmt,
		int flags )
{
	char *expected = NULL;
	char *operation = NULL;
	hebi_z r, a;
	uint64_t b;
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
		if ((flags & LHS_NONZERO) && hebi_zzero(a))
			continue;

		check_pass = 0;
		for ( ; check_pass < check_num_u64values; check_pass++) {
			b = check_u64values[check_pass];
			if ((flags & RHS_NONZERO) && !b)
				continue;

			/* get expected result and operation strings */
			x = aschkprintf(&operation, opfmt, a,
					(unsigned long long)b);
			assert(x >= 0 && operation);
			expected = bcputs(operation);
			assert(expected);

			/* read-only inputs */
			zdirty(r, a, NULL);
			(*f)(r, a, b);
			zcheckstr(r, expected, operation);

			/* inplace operation on lhs input */
			hebi_zset(r, a);
			zdirty(r, NULL);
			(*f)(r, r, b);
			zcheckstr(r, expected, operation);

			/* cleanup this iteration */
			free(expected);
			free(operation);
		}
	}

	hebi_zdestroy(r);
	hebi_zdestroy(a);
}
