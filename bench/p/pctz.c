/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile size_t result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 32768 };
	hebi_packet *a;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	hebi_pzero(a, p.an);
	a->hp_limbs32[p.an + HEBI_PACKET_LIMBS32 - 1] = 0x80000000u;

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_pctz(a, p.an);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);

	return 0;
}
