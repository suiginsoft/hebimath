/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static hebi_alloc_id
getallocidx(int index)
{
	struct hebi_context *ctx;
	hebi_alloc_id rid;
	
	ctx = hebi_context_get();
	rid = ctx->allocids[index];
	if ((intptr_t)rid <= 0)
		rid = HEBI_ALLOC_STDLIB;
	return rid;
}

HEBI_API
hebi_alloc_id
hebi_alloc_get_default(void)
{
	return getallocidx(0);
}

HEBI_API
hebi_alloc_id
hebi_alloc_get_scratch(void)
{
	return getallocidx(1);
}
