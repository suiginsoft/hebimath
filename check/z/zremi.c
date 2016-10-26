#include "../check.h"

static void
zremi(hebi_zptr r, hebi_zsrcptr a, int64_t b)
{
	hebi_zseti(r, hebi_zremi(a, b));
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopi64(zremi, "%Z %% %lld", RHS_NONZERO);
	return 0;
}
