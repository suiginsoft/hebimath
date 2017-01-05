/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"
#include <limits.h>
#include <string.h>

#define COUNT 1024
#define GUARD 8
#define TOTAL (COUNT+GUARD*2)

int main(int argc, char *argv[])
{
	hebi_packet *x;
	size_t i, j, k;

	checkinit(argc, argv);

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, TOTAL);
	assert(x);

	for (k = 1; k <= COUNT; k++) {
		memset(x, UCHAR_MAX, (k+GUARD*2)*sizeof(hebi_packet));

		hebi_pzero(x+GUARD, k);

		for (i = 0; i < k; i++)
			for (j = 0; j < HEBI_PACKET_LIMBS32; j++)
				assert(x[i+GUARD].hp_limbs32[j] == 0);

		for (i = 0; i < GUARD; i++) {
			for (j = 0; j < HEBI_PACKET_LIMBS32; j++) {
				assert(x[i].hp_limbs32[j] == UINT32_MAX);
				assert(x[i+k+GUARD].hp_limbs32[j] == UINT32_MAX);
			}
		}
	}

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, TOTAL);
	return 0;
}
