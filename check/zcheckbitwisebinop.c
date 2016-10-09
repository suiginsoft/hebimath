/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"

static char bcbitwiseops[] =
"define abs(a) {\n"
"	if (a < 0)\n"
"		a = -a\n"
"	return a\n"
"}\n"
"define and(a, b) {\n"
"	auto i, r, s, t, u\n"
"	if (a == 0 || b == 0)\n"
"		return 0\n"
"	s = 1\n"
"	if (a < 0 && b < 0)\n"
"		s = -1\n"
"	a = abs(a)\n"
"	b = abs(b)\n"
"	i = 1\n"
"	r = 0\n"
"	while (a && b) {\n"
"		t = a % 2\n"
"		a /= 2\n"
"		u = b % 2\n"
"		b /= 2\n"
"		if (t && u)\n"
"			r += i\n"
"		i *= 2\n"
"	}\n"
"	return r * s\n"
"}\n"
"define or(a, b) {\n"
"	auto i, r, s, t, u\n"
"	if (a == 0 || b == 0)\n"
"		return 0\n"
"	s = 1\n"
"	if (a < 0 || b < 0)\n"
"		s = -1\n"
"	a = abs(a)\n"
"	b = abs(b)\n"
"	i = 1\n"
"	r = 0\n"
"	while (a || b) {\n"
"		t = a % 2\n"
"		a /= 2\n"
"		u = b % 2\n"
"		b /= 2\n"
"		if (t || u)\n"
"			r += i\n"
"		i *= 2\n"
"	}\n"
"	return r * s\n"
"}\n";

static int initialized;

void
zcheckbitwisebinop(
		void (*f)(hebi_zptr, hebi_zsrcptr, hebi_zsrcptr),
		const char* op )
{
	hebi_z r, a, b;

	if (!initialized) {
		bcwrite(bcbitwiseops);
		initialized = 1;
	}

	hebi_zinits(r, a, b, NULL);

	if (check_max_perm <= 0)
		check_max_perm = 48;
	check_max_iter = check_max_perm * check_max_perm;

	for (check_iter = check_start_iter;
			check_iter < check_max_iter;
			check_iter++) {
		/* generate test inputs */
		zpermutation(check_iter, check_max_perm, 2, a, b);

		/* read-only inputs */
		check_pass = 0;
		zdirty(r, a, b, NULL);
		(*f)(r, a, b);
		zcheckbc(r, "%s(%Z, %Z)", op, a, b);

		/* inplace operation on lhs input */
		check_pass = 1;
		hebi_zset(r, a);
		zdirty(r, a, b, NULL);
		(*f)(a, a, b);
		zcheckbc(a, "%s(%Z, %Z)", op, r, b);
		hebi_zset(a, r);

		/* inplace operation on rhs input */
		check_pass = 2;
		hebi_zset(r, b);
		zdirty(r, a, b, NULL);
		(*f)(b, a, b);
		zcheckbc(b, "%s(%Z, %Z)", op, a, r);
		hebi_zset(b, r);
	}

	hebi_zdestroys(r, a, b, NULL);
}
