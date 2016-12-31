/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char and_script[] =
"define not(a) {\n"
"	return -(a + 1)\n"
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
