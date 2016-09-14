/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#if !defined USE_THREAD_LOCAL && defined USE_THREADS
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

#if defined USE_C11_THREAD_LOCAL

EXTENSION HEBI_HIDDEN _Thread_local struct hebi_context hebi_context__;

#elif defined USE_GNUC_THREAD_LOCAL

EXTENSION HEBI_HIDDEN __thread struct hebi_context hebi_context__;

#elif defined USE_THREADS

#if defined USE_C11_THREADS
static once_flag ctxonce = ONCE_FLAG_INIT
static tss_t ctxkey;
#elif defined USE_POSIX_THREADS
static pthread_once_t ctxonce = PTHREAD_ONCE_INIT;
static pthread_key_t ctxkey;
#endif

static int ctxkeyactive;
static int ctxkeyerr;

static void
ctxkeyinit(void)
{
	int e;

#if defined USE_C11_THREADS

	e = tss_create(&ctxkey, free);
	if (UNLIKELY(e != thrd_success)) {
		ctxkeyerr = errno;
		if (!ctxkeyerr)
			ctxkeyerr = EINVAL;
		return;
	}

#elif defined USE_POSIX_THREADS

	e = pthread_key_create(&ctxkey, free);
	if (UNLIKELY(e)) {
		ctxkeyerr = e;
		return;
	}

#endif

	ctxkeyactive = 1;
	ctxkeyerr = 0;
}

static struct hebi_context *
ctxinit(void)
{
	void *p;
	int e;

	p = calloc(1, sizeof(struct hebi_context));
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

#ifdef USE_GLOBAL_DESTRUCTORS

HEBI_DESTRUCTOR
static void
ctxshut(void)
{
	if (ctxkeyactive) {
		ctxkeyactive = 0;
		ctxkeyerr = EFAULT;
#if defined USE_C11_THREADS
		tss_delete(ctxkey);
#elif defined USE_POSIX_THREADS
		(void)pthread_key_delete(ctxkey);
#endif
	}
}

#endif /* USE_GLOBAL_DESTRUCTORS */

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

HEBI_HIDDEN HEBI_PURE HEBI_WARNUNUSED
struct hebi_context *
hebi_context_get__(hebi_errhandler handler, void *arg)
{
	struct hebi_context *ctx;

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

#else /* !defined USE_THREADS */

HEBI_HIDDEN struct hebi_context hebi_context__;

#endif /* !defined USE_THREADS */
