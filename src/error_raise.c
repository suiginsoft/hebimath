/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <stdlib.h>

HEBI_API HEBI_NORETURN
void
hebi_error_raise(enum hebi_errdom domain, int code)
{
	struct hebi_context *ctx;
	unsigned int i;

	ctx = hebi_context_get();

	if (ctx->scratch) {
		hebi_free(ctx->allocids[1], ctx->scratch, ctx->scratchsize);
		ctx->scratch = NULL;
		ctx->scratchsize = 0;
	}

	for (i = ctx->zstackused; i--; ) {
		hebi_zdestroy(ctx->zstack[i]);
		ctx->zstack[i] = NULL;
	}
	ctx->zstackused = 0;

	if (ctx->errhandler) {
		ctx->errlast.he_domain = domain;
		ctx->errlast.he_code = code;
		ctx->errhandler(ctx->errarg, &ctx->errlast);
	}

	// TODO: print error message to stderr
	abort();
}
