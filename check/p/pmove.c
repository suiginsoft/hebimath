#include "../check.h"
#include <string.h>

#define COUNT 1024
#define GUARD 8
#define TOTAL (COUNT+GUARD*2)

static const uint32_t a[4] = { 0xECECECEC, 0x38383838, 0x92929292, 0xA7A7A7A7 };
static const uint32_t b[4] = { 0x3D3D3D3D, 0x6B6B6B6B, 0x71717171, 0xFEFEFEFE };

int main(int argc, char *argv[])
{
	hebi_packet *x, *y;
	size_t i, j, k;

	x = hebi_palloc(HEBI_ALLOC_DEFAULT, TOTAL);
	y = hebi_palloc(HEBI_ALLOC_DEFAULT, TOTAL);
	assert(x && y);

	for (k = 1; k <= COUNT; k++) {
		for (i = 0; i < k+GUARD*2; i++) {
			for (j = 0; j < HEBI_PACKET_LIMBS32; j++) {
				x[i].hp_limbs32[j] = a[(k+j)&7];
				y[i].hp_limbs32[j] = b[(k+j)&7];
			}
		}

		assert(memcmp(y+GUARD, x+GUARD, k*sizeof(hebi_packet)) != 0);

		hebi_pmove(y+GUARD, x+GUARD, k);

		assert(memcmp(y+GUARD, x+GUARD, k*sizeof(hebi_packet)) == 0);

		for (i = 0; i < GUARD; i++) {
			for (j = 0; j < HEBI_PACKET_LIMBS32; j++) {
				assert(y[i].hp_limbs32[j] == b[(k+j)&7]);
				assert(y[i+k+GUARD].hp_limbs32[j] == b[(k+j)&7]);
			}
		}
	}

	hebi_pfree(HEBI_ALLOC_DEFAULT, x, TOTAL);
	hebi_pfree(HEBI_ALLOC_DEFAULT, y, TOTAL);
	return 0;
}
