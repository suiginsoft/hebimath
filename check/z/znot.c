/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char and_script[] =
"define not(a) {\n"
"	auto r, s, t, u\n"
"	s = 1\n"
"	if (a < 0) {\n"
"		a = -a\n"
"		s = -1\n"
"	}\n"
"	r = 0\n"
"	u = 1\n"
"	while (a) {\n"
"		t = a % 2\n"
"		a /= 2\n"
"		if (!t)\n"
"			r += u\n"
"		u *= 2\n"
"	}\n"
"	return r * -s\n"
"}\n";

int
main(int argc, char *argv[])
{
	check_max_perm = 64;
	check_scale_perm = 2;
	checkinit(argc, argv);
	bcwrite(and_script);
	zcheckunaryop(hebi_znot, "not(%Z)", 0);
	return 0;
}
