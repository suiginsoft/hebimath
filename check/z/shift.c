/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

int
main(int argc, char *argv[])
{
	size_t b, l, n;
	hebi_z q, r, a;
	const hebi_zptr v[3] = { q, r, a };

	check_max_perm = 64;
	check_scale_perm = 4;
	checkinit(argc, argv);

	hebi_zinitv(3, v);

	check_iter = check_first_iter;
	check_max_iter = check_max_perm;
	for ( ; check_iter < check_max_iter; check_iter++) {
		/* generate test inputs */
		zpermutation(check_iter, check_max_perm, 1, a);
		n = hebi_zbits(a);
		l = hebi_zlsb(a);

		check_pass = 0;
		for ( ; check_pass < 1024; check_pass++) {
			b = (size_t)check_pass;

			/* left shift, read-only input */
			zdirty(r, a, NULL);
			hebi_zshl(r, a, b);
			zcheckbc(r, "%Z*(2^%zu)", a, b);

			/* left shift, inplace operation */
			hebi_zset(q, a);
			zdirty(q, NULL);
			hebi_zshl(q, q, b);
			assert(hebi_zcmp(q, r) == 0);

			/* check bit length and trailing zero count */
			assert(hebi_zbits(r) == (n ? b + n : 0));
			assert(hebi_zlsb(r) == (n ? b + l : 0));

			/* right shift, read-only input */
			zdirty(q, NULL);
			hebi_zshr(r, q, b);
			assert(hebi_zcmp(r, a) == 0);

			/* right shift, inplace operation */
			hebi_zset(r, q);
			zdirty(r, NULL);
			hebi_zshr(r, r, b);
			assert(hebi_zcmp(a, r) == 0);
		}
	}

	hebi_zdestroyv(3, v);

	return 0;
}
