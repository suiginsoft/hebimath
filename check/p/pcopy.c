#include "../check.h"
#include <string.h>

#define NUM_PACKETS 501
#define NUM_DIGITS (NUM_PACKETS * HEBI_PACKET_WORDS)
#define NUM_BYTES (NUM_PACKETS * sizeof(hebi_packet))

int main(int argc, char *argv[])
{
	hebi_packet *x, *y, *z;
	size_t i, j;

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	y = hebi_palloc(HEBI_ALLOC_DEFAULT, NUM_PACKETS);
	assert(x && y);

	for (i = 0; i < NUM_PACKETS; ++i) {
		for (j = 0; j < HEBI_PACKET_WORDS; ++j) {
			x[i].hp_words[j] = (hebi_word)i;
			y[i].hp_words[j] = (hebi_word)i + 55;
		}
	}

	assert(memcmp(x, y, NUM_BYTES) != 0);
	z = hebi_pcopy(y, x, NUM_PACKETS);
	assert(z == y + NUM_PACKETS);
	assert(memcmp(x, y, NUM_BYTES) == 0);

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, NUM_PACKETS);
	hebi_pfree(HEBI_ALLOC_DEFAULT, y, NUM_PACKETS);
	return 0;
}
