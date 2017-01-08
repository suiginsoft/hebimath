#include "../check.h"

#define MAX_PACKETS 64
#define MAX_ITERATIONS 256

static struct hebi_kiss kiss = HEBI_KISS_INIT;

int
main(int argc, char *argv[])
{
	hebi_packet *x, *y, *z;
	size_t i, n, nbits;
	uint64_t c, d;

	checkinit(argc, argv);

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, MAX_PACKETS);
	y = hebi_palloc(HEBI_ALLOC_DEFAULT, MAX_PACKETS);
	z = hebi_palloc(HEBI_ALLOC_DEFAULT, MAX_PACKETS);
	assert(x && y && z);

	for (i = 0; i < MAX_ITERATIONS * MAX_PACKETS; i++) {
		/* get size of arguments from iteration counter */
		n = (i / MAX_ITERATIONS) + 1;
		nbits = n * HEBI_PACKET_BIT;

		/* generate random value */
		hebi_prand_kiss(z, MAX_PACKETS, nbits, &kiss);

		/* negation */
		c = hebi_pneg(x, z, n);

		/* two's complement */
		hebi_pnot(y, z, n);
		d = !hebi_paddu(y, y, 1, n);

		assert(hebi_pcmp(x, y, n) == 0);
		assert(c == d);
	}

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, MAX_PACKETS);
	hebi_pfree(HEBI_ALLOC_DEFAULT, y, MAX_PACKETS);
	hebi_pfree(HEBI_ALLOC_DEFAULT, z, MAX_PACKETS);

	return 0;
}
