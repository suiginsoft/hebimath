/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static const char cmp_script[] =
"define cmp(a, b) {\n"
"	if (a < b)\n"
"		return -1\n"
"	if (a > b)\n"
"		return 1\n"
"	return 0\n"
"}\n";

static void zcmpi_wrapper(hebi_zptr r, hebi_zsrcptr a, int64_t b)
{
	hebi_zseti(r, hebi_zcmpi(a, b));
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(cmp_script);
	zcheckbinopi64(zcmpi_wrapper, "cmp(%Z, %lld)", 0);
	return 0;
}
