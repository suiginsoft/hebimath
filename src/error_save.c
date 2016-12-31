/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <string.h>

HEBI_API
void
hebi_error_save(struct hebi_errstate *state)
{
	struct hebi_context *ctx = hebi_context_get__();
	state->hes_handler = ctx->errhandler;
	if (ctx->errarg != &ctx->errjmparg) {
		state->hes_arg = ctx->errarg;
		state->hes_longjmp = 0;
		memset(state->hes_env, 0, sizeof(jmp_buf));
		state->hes_val = 0;
	} else {
		state->hes_arg = NULL;
		state->hes_longjmp = 1;
		memcpy(state->hes_env, ctx->errjmparg.env, sizeof(jmp_buf));
		state->hes_val = ctx->errjmparg.val;
	}
}
