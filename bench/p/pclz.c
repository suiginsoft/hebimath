/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile size_t result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 65536 };
	hebi_packet *a;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	hebi_pzero(a, p.an);
	a->hp_limbs64[0] = 1;

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_pclz(a, p.an);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);

	return 0;
}
