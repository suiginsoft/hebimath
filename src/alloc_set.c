/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static hebi_allocid
setallocalias(struct hebi_context *ctx, hebi_allocid id, hebi_allocid aliasid)
{
	const size_t index = hebi_alloc_alias_index__(aliasid);
	hebi_allocid retid;
	int key;

	retid = ctx->allocaliaskeys[index];
	if (retid == 0)
		retid = HEBI_ALLOC_STDLIB;
	else if (UNLIKELY(retid < 0))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADSTATE);

	key = hebi_alloc_query_key__(&ctx, id);
	if (UNLIKELY(key < 0))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	ctx->allocaliaskeys[index] = key;
	return retid;
}

HEBI_API
hebi_allocid
hebi_alloc_set_default(hebi_allocid id)
{
	return setallocalias(hebi_context_get__(), id, HEBI_ALLOC_DEFAULT);
}

HEBI_API
hebi_allocid
hebi_alloc_set_scratch(hebi_allocid id)
{
	struct hebi_context *ctx = hebi_context_get__();
	(void)hebi_realloc_scratch__(ctx, 0);
	return setallocalias(ctx, id, HEBI_ALLOC_SCRATCH);
}
