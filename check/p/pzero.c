#include "../check.h"
#include <limits.h>
#include <string.h>

#define NUM_PACKETS 501
#define NUM_DIGITS (NUM_PACKETS * HEBI_PACKET_LIMBS64)
#define NUM_BYTES (NUM_PACKETS * sizeof(hebi_packet))

int main(int argc, char *argv[])
{
	hebi_packet *x;
	size_t i, j;

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	assert(x);

	memset(x, UCHAR_MAX, NUM_BYTES);

	hebi_pzero(x, NUM_PACKETS);

	for (i = 0; i < NUM_PACKETS; ++i)
		for (j = 0; j < HEBI_PACKET_LIMBS64; ++j)
			assert(x[i].hp_limbs64[j] == 0);

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, NUM_PACKETS);

	return 0;
}
