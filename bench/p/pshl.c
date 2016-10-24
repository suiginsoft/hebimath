/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile size_t result_sink;
static const size_t b = 443;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 65536 };
	hebi_packet *a, *r;
	size_t rn;
	int i;

	bench_init(argc, argv, &p);

	rn = p.an + 2;
	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	r = hebi_palloc(HEBI_ALLOC_DEFAULT, rn);
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);
	hebi_pzero(r, rn);

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_pshl(r, a, b, p.an);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, r, rn);

	return 0;
}
