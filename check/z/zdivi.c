#include "../check.h"

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopi64(hebi_zdivi, "/", RHS_NONZERO);
	return 0;
}
