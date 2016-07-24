/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinop(hebi_zadd, "+", 0);
	return 0;
}
