/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static hebi_packet *result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 32768 };
	hebi_packet *a;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_pzero(a, p.an);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);

	return 0;
}
