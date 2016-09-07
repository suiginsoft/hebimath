/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile size_t result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 512, .bn = 256 };
	hebi_packet *a, *b, *q, *r, *w;
	size_t rn;
	int i;

	bench_init(argc, argv, &p);

	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	b = hebi_palloc(HEBI_ALLOC_DEFAULT, p.bn);
	q = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	r = hebi_palloc(HEBI_ALLOC_DEFAULT, p.bn);
	w = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an + p.bn + 2);
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);
	hebi_prand_kiss(b, p.bn, p.bn * HEBI_PACKET_BIT, &bench_kiss);

	bench_start();

	for (i = 0; i < p.iter; ++i)
		result_sink = hebi_pdivrem(q, r, w, &rn, a, b, p.an, p.bn);

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, b, p.bn);
	hebi_pfree(HEBI_ALLOC_DEFAULT, q, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, r, p.bn);
	hebi_pfree(HEBI_ALLOC_DEFAULT, w, p.an + p.bn + 2);

	return 0;
}
