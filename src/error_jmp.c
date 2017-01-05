/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <string.h>

HEBI_NORETURN
static void
longjmphandler(void *arg, const struct hebi_error *err)
{
	IGNORE(err);
	struct hebi_longjmparg *ljarg = arg;
	longjmp(ljarg->env, ljarg->val);
}

HEBI_API
void
hebi_error_jmp(jmp_buf env, int val)
{
	struct hebi_context *ctx;

#if !defined USE_THREAD_LOCAL && defined USE_THREADS
	struct hebi_longjmparg ljarg;
	memcpy(ljarg.env, env, sizeof(jmp_buf));
	ljarg.val = val;
	ctx = hebi_context_get__(&longjmphandler, &ljarg);
#else
	ctx = &hebi_context__;
#endif

	ctx->errhandler = &longjmphandler;
	ctx->errarg = &ctx->errjmparg;
	memcpy(ctx->errjmparg.env, env, sizeof(jmp_buf));
	ctx->errjmparg.val = val;
}
