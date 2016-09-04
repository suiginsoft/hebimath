/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile uint64_t result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 32768, .bn = 16384 };
	hebi_packet *a, *b, *r;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	b = hebi_palloc(HEBI_ALLOC_DEFAULT, p.bn);
	r = hebi_palloc(HEBI_ALLOC_DEFAULT, MAX(p.an, p.bn));
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);
	hebi_prand_kiss(b, p.bn, p.bn * HEBI_PACKET_BIT, &bench_kiss);
	hebi_pzero(r, MAX(p.an, p.bn));

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_psub(r, a, b, p.an, p.bn);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, b, p.bn);
	hebi_pfree(HEBI_ALLOC_DEFAULT, r, MAX(p.an, p.bn));

	return 0;
}
