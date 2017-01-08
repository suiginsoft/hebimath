/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"
#include <limits.h>

int
main(int argc, char *argv[])
{
	assert(hebi_alloc_valid(HEBI_ALLOC_DEFAULT));
	assert(hebi_alloc_valid(HEBI_ALLOC_SCRATCH));
	assert(hebi_alloc_valid(HEBI_ALLOC_STDLIB));
	assert(hebi_alloc_valid(HEBI_ALLOC_FIXED));
	assert(!hebi_alloc_valid(HEBI_ALLOC_INVALID));
	assert(!hebi_alloc_valid(1));
	assert(!hebi_alloc_valid(INT_MAX));
	assert(!hebi_alloc_valid(INT_MIN));

	(void)argc;
	(void)argv;
	return 0;
}
