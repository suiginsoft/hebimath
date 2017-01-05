/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static hebi_allocid
getallocidx(int index)
{
	struct hebi_context *ctx;
	hebi_allocid id;
	
	ctx = hebi_context_get__();
	id = ctx->allocids[index];
	return id > 0 ? id : HEBI_ALLOC_STDLIB;
}

HEBI_API
hebi_allocid
hebi_alloc_get_default(void)
{
	return getallocidx(0);
}

HEBI_API
hebi_allocid
hebi_alloc_get_scratch(void)
{
	return getallocidx(1);
}
