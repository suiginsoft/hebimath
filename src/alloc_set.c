/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static hebi_allocid
setallocidx(struct hebi_context *ctx, hebi_allocid id, int index)
{
	hebi_allocid rid;
	int key;
	
	key = (int)(intptr_t)id;
	if (UNLIKELY(key < -2))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADALLOCID);

	rid = ctx->allocids[index];
	if ((intptr_t)rid <= 0)
		rid = HEBI_ALLOC_STDLIB;

	if ((key += 2) > 0)
		id = ctx->allocids[key & 1];
	else if (!key)
		id = 0;

	ctx->allocids[index] = id;
	return rid;
}

HEBI_API
hebi_allocid
hebi_alloc_set_default(hebi_allocid id)
{
	return setallocidx(hebi_context_get(), id, 0);
}

HEBI_API
hebi_allocid
hebi_alloc_set_scratch(hebi_allocid id)
{
	struct hebi_context *ctx;
	ctx = hebi_context_get();
	(void)hebi_realloc_scratch__(ctx, 0);
	return setallocidx(ctx, id, 1);
}
