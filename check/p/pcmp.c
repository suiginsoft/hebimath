#include "../check.h"
#include <limits.h>
#include <string.h>

#define NUM_PACKETS 501
#define NUM_BYTES (NUM_PACKETS * sizeof(hebi_packet))

int main(int argc, char *argv[])
{
	hebi_packet *x, *y;
	size_t i;

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	y = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	assert(x && y);

	memset(x, UCHAR_MAX, NUM_BYTES);
	memset(y, 0, NUM_BYTES);

	assert(hebi_pcmp(x, x, NUM_PACKETS) == 0);
	assert(hebi_pcmp(y, y, NUM_PACKETS) == 0);
	assert(hebi_pcmp(x, y, NUM_PACKETS) == 1);
	assert(hebi_pcmp(y, x, NUM_PACKETS) == -1);

	memset(y, UCHAR_MAX, NUM_BYTES - sizeof(hebi_packet) / 2);

	for (i = 0; i < sizeof(hebi_packet) / 2; ++i) {
		assert(hebi_pcmp(x, x, NUM_PACKETS) == 0);
		assert(hebi_pcmp(y, y, NUM_PACKETS) == 0);
		assert(hebi_pcmp(x, y, NUM_PACKETS) == 1);
		assert(hebi_pcmp(y, x, NUM_PACKETS) == -1);
		memset((char *)x + NUM_BYTES - i - 1, 0, sizeof(char));
	}

	assert(hebi_pcmp(x, y, NUM_PACKETS) == 0);
	memset((char *)x + NUM_BYTES - i - 1, 0, sizeof(char));

	for (++i; i < sizeof(hebi_packet); ++i) {
		assert(hebi_pcmp(x, x, NUM_PACKETS) == 0);
		assert(hebi_pcmp(y, y, NUM_PACKETS) == 0);
		assert(hebi_pcmp(x, y, NUM_PACKETS) == -1);
		assert(hebi_pcmp(y, x, NUM_PACKETS) == 1);
		memset((char *)x + NUM_BYTES - i - 1, 0, sizeof(char));
	}

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, NUM_PACKETS);
	hebi_pfree(HEBI_ALLOC_DEFAULT, y, NUM_PACKETS);
	return 0;
}
