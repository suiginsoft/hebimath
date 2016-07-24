/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API
void
hebi_error_handler(hebi_errhandler handler, void *arg)
{
	struct hebi_context *ctx;

#if !defined USE_THREAD_LOCAL && defined USE_THREADS
	ctx = hebi_context_get__(handler, arg);
#else
	ctx = &hebi_context__;
#endif

	ctx->errhandler = handler;
	ctx->errarg = arg;
}
