/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile uint64_t result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 256, .bn = 256 };
	hebi_packet *a, *b, *r;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	b = hebi_palloc(HEBI_ALLOC_DEFAULT, p.bn);
	r = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an + p.bn + 1);
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);
	hebi_prand_kiss(b, p.bn, p.bn * HEBI_PACKET_BIT, &bench_kiss);
	hebi_pzero(r, p.an + p.bn + 1);

	bench_start();

	for (i = 0; i < p.iter; ++i) {
		hebi_pmul(r, a, b, p.an, p.bn);
		result_sink = r->hp_limbs64[0];
	}

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, b, p.bn);
	hebi_pfree(HEBI_ALLOC_DEFAULT, r, p.an + p.bn + 1);

	return 0;
}
