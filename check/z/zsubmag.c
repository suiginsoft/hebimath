/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char abs_script[] =
"define abs(a) {\n"
"	if (a < 0)\n"
"		a = -a\n"
"	return a\n"
"}\n";

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(abs_script);
	zcheckbinop(hebi_zsubmag, "abs(%Z) - abs(%Z)", 0);
	return 0;
}
