/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_HIDDEN HEBI_ALLOC
void *
hebi_realloc_scratch__(struct hebi_context *ctx, size_t newsize)
{
#ifdef USE_THREAD_LOCAL
	struct hebi_shadow_context *shadow = NULL;
#endif
	void *p;
	size_t n;
	const struct hebi_allocfnptrs *fp;

	p = ctx->scratch;
	if (p) {
		n = ctx->scratchsize;
		fp = ctx->scratchfp;
		ASSERT(fp);

		ctx->scratchfp = NULL;
		ctx->scratch = NULL;
		ctx->scratchsize = 0;

#ifdef USE_THREAD_LOCAL
		shadow = ctx->shadow;
		if (shadow) {
			shadow->scratchfp = NULL;
			shadow->scratch = NULL;
			shadow->scratchsize = 0;
		}
#endif

		hebi_freefp(fp, p, n);
	}

	if (newsize > 0) {
		fp = hebi_alloc_query(NULL, HEBI_ALLOC_SCRATCH);
		p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, newsize);

		ctx->scratchfp = fp;
		ctx->scratch = p;
		ctx->scratchsize = newsize;

#ifdef USE_THREAD_LOCAL
		if (!shadow)
			shadow = hebi_shadow_context_get__(ctx);
		shadow->scratchfp = fp;
		shadow->scratch = p;
		shadow->scratchsize = newsize;
#endif
	} else {
		p = NULL;
	}

	return p;
}
