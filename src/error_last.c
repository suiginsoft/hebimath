/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

#if !defined USE_THREAD_LOCAL && defined USE_THREADS

struct lastarg {
	struct hebi_error *err;
	jmp_buf env;
};

HEBI_NORETURN
static void
lasthandler(void *arg, const struct hebi_error *err)
{
	struct lastarg *larg = arg;
	if (larg->err)
		*larg->err = *err;
	longjmp(larg->env, 1);
}

#endif

HEBI_API HEBI_NOTHROW
int
hebi_error_last(struct hebi_error *err)
{
	struct hebi_context *ctx;

#if !defined USE_THREAD_LOCAL && defined USE_THREADS
	struct lastarg larg;
	larg.err = err;
	if (UNLIKELY(setjmp(larg.env)))
		return -1;
	ctx = hebi_context_get__(&lasthandler, &larg);
#else
	ctx = &hebi_context__;
#endif

	if (err)
		*err = ctx->errlast;
	return !ctx->errlast.he_code ? 0 : -1;
}
