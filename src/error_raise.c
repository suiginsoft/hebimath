/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef USE_C11_THREADS
#include <threads.h>
#endif

#define ESTR(E) \
	case E: \
		estr = STRINGIZE(E); \
		break;

#define ESTRDEFAULT() \
	default: \
		estr = "unknown"; \
		break;

HEBI_API HEBI_NORETURN
void
hebi_error_raise(enum hebi_errdom domain, int code)
{
	struct hebi_context *ctx;
	const char *estr;
	unsigned int i;

	ctx = hebi_context_get__();
	hebi_realloc_scratch__(ctx, 0);

	for (i = ctx->zstackused; i > 0; i--) {
		hebi_zdestroy(ctx->zstack[i - 1]);
		ctx->zstack[i - 1] = NULL;
	}
	ctx->zstackused = 0;

	if (ctx->errhandler) {
		ctx->errlast.he_domain = domain;
		ctx->errlast.he_code = code;
		ctx->errhandler(ctx->errarg, &ctx->errlast);
	}

	if (domain == HEBI_ERRDOM_HEBI) {
		switch (code) {
			ESTR(HEBI_ENONE)
			ESTR(HEBI_EDIVZERO)
			ESTR(HEBI_EZERODIVZERO)
			ESTR(HEBI_EZEROPOWZERO)
			ESTR(HEBI_EASSERTION)
			ESTR(HEBI_EBADLENGTH)
			ESTR(HEBI_EBADSTATE)
			ESTR(HEBI_EBADVALUE)
			ESTR(HEBI_ENOHWCAPS)
			ESTR(HEBI_ENOMEM)
			ESTR(HEBI_ENOSLOTS)
			ESTRDEFAULT()
		}
		fprintf(stderr, "hebimath error: %s\n", estr);
	} else if (domain == HEBI_ERRDOM_THRD) {
#ifdef USE_C11_THREADS
		switch (code) {
			ESTR(thrd_success)
			ESTR(thrd_timedout)
			ESTR(thrd_busy)
			ESTR(thrd_nomem)
			ESTR(thrd_error)
			ESTRDEFAULT()
		}
		fprintf(stderr, "hebimath threading error: %s\n", estr);
#else
		fprintf(stderr, "hebimath threading error: %d\n", code);
#endif
	} else if (domain == HEBI_ERRDOM_ERRNO) {
		errno = code;
		perror("hebimath");
	}

	abort();
}
