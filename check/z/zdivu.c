#include "../check.h"

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopu64(hebi_zdivu, "%Z / %llu", RHS_NONZERO);
	return 0;
}
