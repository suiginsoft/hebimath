/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"
#include <limits.h>
#include <string.h>

#define NUM_PACKETS 501
#define NUM_BYTES (NUM_PACKETS * sizeof(hebi_packet))

int main(int argc, char *argv[])
{
	size_t i, expected, actual;
	hebi_packet *p;

	p = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	assert(p);

	memset(p, UCHAR_MAX, NUM_BYTES);

	for (i = 0; i < NUM_BYTES; ++i) {
		expected = ((NUM_BYTES - i) + (sizeof(hebi_packet) - 1)) /
			sizeof(hebi_packet);
		actual = hebi_pnorm(p, NUM_PACKETS);
		assert(expected == actual);
		memset((char *)p + (NUM_BYTES - i - 1), 0, sizeof(char));
	}

	hebi_pfree(HEBI_ALLOC_DEFAULT, p, NUM_PACKETS);
	return 0;
}
