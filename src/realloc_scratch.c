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

	const struct hebi_allocfnptrs *fp;
	void *p;
	size_t n;

	ASSERT(ctx);

	if ((p = ctx->scratch)) {
		fp = ctx->scratchfp;
		n = ctx->scratchsize;
		ASSERT(fp);

		ctx->scratchfp = NULL;
		ctx->scratch = NULL;
		ctx->scratchsize = 0;

#ifdef USE_THREAD_LOCAL
		if ((shadow = ctx->shadow)) {
			shadow->scratchfp = NULL;
			shadow->scratch = NULL;
			shadow->scratchsize = 0;
		}
#endif

		hebi_freefp(fp, p, n);
	}

	if (newsize > 0) {
		fp = hebi_alloc_query(NULL, ctx->allocids[1]);
		p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, newsize);
	
		ctx->scratchfp = fp;
		ctx->scratch = p;
		ctx->scratchsize = newsize;

#ifdef USE_THREAD_LOCAL
		if (!shadow || !(shadow = ctx->shadow))
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
