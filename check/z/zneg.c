/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckunaryop(hebi_zneg, "-(%Z)", 0);
	return 0;
}
