#include "../check.h"

static const char cmp_script[] =
"define cmp(a, b) {\n"
"	if (a < b)\n"
"		return -1\n"
"	if (a > b)\n"
"		return 1\n"
"	return 0\n"
"}\n";

static void zcmpu_wrapper(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	hebi_zseti(r, hebi_zcmpu(a, b));
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(cmp_script);
	zcheckbinopu64(zcmpu_wrapper, "cmp(%Z, %llu)", 0);
	return 0;
}
