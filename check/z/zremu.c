#include "../check.h"

static void
zremu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	int s = hebi_zsign(a);
	hebi_zsetu(r, hebi_zremu(a, b));
	if (s < 0)
		hebi_zneg(r, r);
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopu64(zremu, "%Z %% %llu", RHS_NONZERO);
	return 0;
}
