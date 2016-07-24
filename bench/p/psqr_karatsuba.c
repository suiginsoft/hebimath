/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../bench.h"

static volatile hebi_word result_sink;

int
main(int argc, char *argv[])
{
	bench_args p = { .iter = 5000, .an = 256 };
	hebi_packet *a, *r, *w;
	size_t rn, wn;
	int i;

	bench_init(argc, argv, &p);

	rn = 2 * p.an + 1;
	wn = hebi_psqr_karatsuba_space(p.an);
	a = hebi_palloc(HEBI_ALLOC_DEFAULT, p.an);
	r = hebi_palloc(HEBI_ALLOC_DEFAULT, rn);
	w = hebi_palloc(HEBI_ALLOC_DEFAULT, wn);
	hebi_prand_kiss(a, p.an, p.an * HEBI_PACKET_BIT, &bench_kiss);

	bench_start();

	for (i = 0; i < p.iter; ++i) {
		hebi_pzero(r, rn);
		hebi_psqr_karatsuba(r, w, a, p.an);
		result_sink = r->hp_words[0];
	}

	bench_stop();

	hebi_pfree(HEBI_ALLOC_DEFAULT, a, p.an);
	hebi_pfree(HEBI_ALLOC_DEFAULT, r, rn);
	hebi_pfree(HEBI_ALLOC_DEFAULT, w, wn);

	return 0;
}
