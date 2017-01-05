/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <string.h>

HEBI_API
void
hebi_error_restore(const struct hebi_errstate *state)
{
	struct hebi_context *ctx = hebi_context_get__();
	ctx->errhandler = state->hes_handler;
	if (state->hes_longjmp) {
		ctx->errarg = &ctx->errjmparg;
		ctx->errjmparg.val = state->hes_val;
		memcpy(ctx->errjmparg.env, state->hes_env, sizeof(jmp_buf));
	} else {
		ctx->errarg = state->hes_arg;
	}
}
