/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#ifdef USE_THREADS
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#if defined USE_C11_THREADS
#include <threads.h>
#elif defined USE_POSIX_THREADS
#include <pthread.h>
#endif
#endif

STATIC_ASSERT(CACHE_LINE_SIZE > 0, "must be greater than zero");
STATIC_ASSERT((CACHE_LINE_SIZE & (CACHE_LINE_SIZE - 1)) == 0, "must be a power of two");

STATIC_ASSERT(ZSTACK_MAX_SIZE > 0, "must be greater than zero");

#ifdef USE_THREADS

#if defined USE_C11_THREADS
static once_flag ctxonce = ONCE_FLAG_INIT
static tss_t ctxkey;
#elif defined USE_POSIX_THREADS
static pthread_once_t ctxonce = PTHREAD_ONCE_INIT;
static pthread_key_t ctxkey;
#endif

static int ctxkeyactive;
static int ctxkeyerr;

static void *
ctxinit(void)
{
	void *p;
	int e;

#ifdef USE_THREAD_LOCAL
	p = calloc(1, sizeof(struct hebi_shadow_context));
#else
	p = calloc(1, sizeof(struct hebi_context));
#endif

	if (UNLIKELY(!p)) {
		if (UNLIKELY(!errno))
			errno = ENOMEM;
		return NULL;
	}

#if defined USE_C11_THREADS

	e = tss_set(ctxkey, p);
	if (UNLIKELY(e != thrd_success)) {
		free(p);
		errno = EINVAL;
		return NULL;
	}

#elif defined USE_POSIX_THREADS

	e = pthread_setspecific(ctxkey, p);
	if (UNLIKELY(e)) {
		free(p);
		errno = e;
		return NULL;
	}

#endif

	return p;
}

static void
ctxshut(void *arg)
{
#ifdef USE_THREAD_LOCAL
	struct hebi_shadow_context *ctx = arg;
#else
	struct hebi_context *ctx = arg;
#endif

	const struct hebi_allocfnptrs *fp;
	void *p;
	size_t n;

	if (ctx) {
		if ((p = ctx->scratch)) {
			fp = ctx->scratchfp;
			n = ctx->scratchsize;
			ASSERT(fp);

			ctx->scratchfp = NULL;
			ctx->scratch = NULL;
			ctx->scratchsize = 0;

			hebi_freefp(fp, p, n);
		}

#ifdef USE_ASSERTIONS
		ASSERT(!ctx->zstackused);
#endif

		free(ctx);
	}
}

static void
ctxkeyinit(void)
{
	int e;

#if defined USE_C11_THREADS

	e = tss_create(&ctxkey, ctxshut);
	if (UNLIKELY(e != thrd_success)) {
		ctxkeyerr = errno;
		if (!ctxkeyerr)
			ctxkeyerr = EINVAL;
		return;
	}

#elif defined USE_POSIX_THREADS

	e = pthread_key_create(&ctxkey, ctxshut);
	if (UNLIKELY(e)) {
		ctxkeyerr = e;
		return;
	}

#endif

	ctxkeyactive = 1;
	ctxkeyerr = 0;
}

HEBI_NORETURN
static void
ctxraise(hebi_errhandler handler, void *arg, int e)
{
	struct hebi_error err;

	if (handler) {
		err.he_domain = HEBI_ERRDOM_ERRNO;
		err.he_code = e;
		handler(arg, &err);
	}

	errno = e;
	perror("hebimath");
	abort();
}

HEBI_PURE HEBI_WARNUNUSED
static void *
ctxgetorcreate(hebi_errhandler handler, void *arg)
{
	void *ctx;

#if defined USE_C11_THREADS

	call_once(&ctxonce, ctxkeyinit);
	if (UNLIKELY(ctxkeyerr))
		ctxraise(handler, arg, ctxkeyerr);
	ctx = tss_get(ctxkey);

#elif defined USE_POSIX_THREADS

	int e;

	e = pthread_once(&ctxonce, ctxkeyinit);
	if (UNLIKELY(e))
		ctxraise(handler, arg, e);
	if (UNLIKELY(ctxkeyerr))
		ctxraise(handler, arg, ctxkeyerr);
	ctx = pthread_getspecific(ctxkey);

#endif

	if (UNLIKELY(!ctx)) {
		ctx = ctxinit();
		if (UNLIKELY(!ctx))
			ctxraise(handler, arg, errno);
	}

	return ctx;
}

#ifdef USE_THREAD_LOCAL

#if defined USE_C11_THREAD_LOCAL

EXTENSION HEBI_HIDDEN _Thread_local struct hebi_context hebi_context__;

#elif defined USE_GNUC_THREAD_LOCAL

EXTENSION HEBI_HIDDEN __thread struct hebi_context hebi_context__;

#endif

HEBI_HIDDEN HEBI_PURE HEBI_WARNUNUSED
struct hebi_shadow_context *
hebi_shadow_context_get_or_create__(struct hebi_context *ctx)
{
	ASSERT(ctx);

	if (!ctx->shadow)
		ctx->shadow = ctxgetorcreate(NULL, NULL);

	return ctx->shadow;
}

#else /* USE_THREAD_LOCAL */

HEBI_HIDDEN HEBI_PURE HEBI_WARNUNUSED
struct hebi_context *
hebi_context_get_or_create__(hebi_errhandler handler, void *arg)
{
	return ctxgetorcreate(handler, arg);
}

#endif /* USE_THREAD_LOCAL */

#else /* USE_THREADS */

HEBI_HIDDEN struct hebi_context hebi_context__;

#endif /* USE_THREADS */

#ifdef USE_GLOBAL_DESTRUCTORS

HEBI_DESTRUCTOR
static void
global_shutdown(void)
{
	struct hebi_context* ctx = hebi_context_get__();
	(void)hebi_realloc_scratch__(ctx, 0);
	ASSERT(!ctx->zstackused);

#ifdef USE_THREADS

	if (ctxkeyactive) {
		ctxkeyactive = 0;
		ctxkeyerr = EFAULT;
#if defined USE_C11_THREADS
		tss_delete(ctxkey);
#elif defined USE_POSIX_THREADS
		(void)pthread_key_delete(ctxkey);
#endif
	}

#endif

	hebi_alloc_table_shut__();
}

#endif /* USE_GLOBAL_DESTRUCTORS */
