/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopu64(hebi_zaddu, "+", 0);
	return 0;
}
