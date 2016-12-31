/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char cmpmag_script[] =
"define cmpmag(a, b) {\n"
"	if (a < 0)\n"
"		a = -a\n"
"	if (b < 0)\n"
"		b = -b\n"
"	if (a < b)\n"
"		return -1\n"
"	if (a > b)\n"
"		return 1\n"
"	return 0\n"
"}\n";

static void zcmpmag_wrapper(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_zseti(r, hebi_zcmpmag(a, b));
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(cmpmag_script);
	zcheckbinop(zcmpmag_wrapper, "cmpmag(%Z, %Z)", 0);
	return 0;
}
