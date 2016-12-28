/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define ESTR(E) case E: estr = #E; break;

HEBI_API HEBI_NORETURN
void
hebi_error_raise(enum hebi_errdom domain, int code)
{
	const char *estr;
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

	if (domain == HEBI_ERRDOM_HEBI) {
		switch (code) {
			ESTR(HEBI_EDIVZERO)
			ESTR(HEBI_EZERODIVZERO)
			ESTR(HEBI_EZEROPOWZERO)
			ESTR(HEBI_EBADALLOCID)
			ESTR(HEBI_EBADLENGTH)
			ESTR(HEBI_EBADVALUE)
			ESTR(HEBI_ENOHWCAPS)
			ESTR(HEBI_ENOMEM)
			ESTR(HEBI_ENOSLOTS)
			ESTR(HEBI_EASSERTION)
			default: estr = "unknown"; break;
		}
		fprintf(stderr, "hebimath error: %s\n", estr);
	} else {
		errno = code;
		perror("hebimath");
	}

	abort();
}
