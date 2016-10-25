/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char sqr_script[] =
"define sqr(a) {\n"
"	return a * a\n"
"}\n";

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(sqr_script);
	zcheckunaryop(hebi_zsqr, "sqr(%Z)", 0);
	return 0;
}
