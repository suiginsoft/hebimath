#include "../check.h"

static const char cmp_script[] =
"define cmp(a, b) {\n"
"	if (a < b)\n"
"		return -1\n"
"	if (a > b)\n"
"		return 1\n"
"	return 0\n"
"}\n";

static void zcmp_wrapper(hebi_zptr r, hebi_zsrcptr a, hebi_zsrcptr b)
{
	hebi_zseti(r, hebi_zcmp(a, b));
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	bcwrite(cmp_script);
	zcheckbinop(zcmp_wrapper, "cmp(%Z, %Z)", 0);
	return 0;
}
