/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static hebi_allocid
setallocidx(struct hebi_context *ctx, hebi_allocid id, int index)
{
	hebi_allocid retid;
	hebi_allocid newid;
	unsigned int key;
	
	if (UNLIKELY(id < -2))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADALLOCID);

	retid = ctx->allocids[index];
	if (retid <= 0)
		retid = HEBI_ALLOC_STDLIB;

	newid = id;
	key = (unsigned int)(id + 2);
	if (key > 0)
		newid = ctx->allocids[key & 1];
	else if (!key)
		newid = 0;

	ctx->allocids[index] = newid;
	return retid;
}

HEBI_API
hebi_allocid
hebi_alloc_set_default(hebi_allocid id)
{
	return setallocidx(hebi_context_get__(), id, 0);
}

HEBI_API
hebi_allocid
hebi_alloc_set_scratch(hebi_allocid id)
{
	struct hebi_context *ctx;
	ctx = hebi_context_get__();
	(void)hebi_realloc_scratch__(ctx, 0);
	return setallocidx(ctx, id, 1);
}
