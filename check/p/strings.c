#include "../check.h"
#include <stdio.h>
#include <stdlib.h>

static struct hebi_kiss kiss = HEBI_KISS_INIT;

/* packet buffers for stress test */
enum { MAX_PACKETS = 16 };
static hebi_packet x[MAX_PACKETS + 1];
static hebi_packet y[MAX_PACKETS + 1];
static hebi_packet z[MAX_PACKETS + 1];

/* room for all base 2 digits, optional radix prefix and null terminator */
enum { MAX_LENGTH = MAX_PACKETS * HEBI_PACKET_BIT + 2 + 1 };
static char str[MAX_LENGTH];

static void
stresstest(unsigned int flags)
{
	size_t i, m, n, bits, len;
	unsigned int base;

	for (i = 0; i < MAX_PACKETS * HEBI_PACKET_BIT; i++) {
		/* get next random value */
		bits = i;
		n = (bits + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;
		if (n > 0) {
			hebi_prand_kiss(x, n, bits, &kiss);
			n = hebi_pnorm(x, n); /* TODO: fix prand */
		}

		/* test converting to string and back for each base */
		for (base = 2; base <= 62 /* TODO: 63 & 64 */; base++) {
			hebi_pcopy(z, x, n);
			len = hebi_pgetstr(str, sizeof(str), z, n, base, flags);
			assert(len > 0 && len < sizeof(str));

			hebi_pzero(y, MAX_PACKETS + 1);
			m = hebi_psetstr(y, MAX_PACKETS + 1, str, len, base, flags);
			assert(m == n && hebi_pcmp(x, y, m) == 0);
		}
	}
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	stresstest(HEBI_STR_CLASSIC_ALPHABET);
	return 0;
}
