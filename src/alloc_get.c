/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

static inline hebi_allocid
getallocalias(hebi_allocid aliasid)
{
	const size_t aliasindex = hebi_alloc_alias_index__(aliasid);
	struct hebi_context *ctx = hebi_context_get__();
	int key = ctx->allocaliaskeys[aliasindex];
	if (key == 0)
		return HEBI_ALLOC_STDLIB;
	else if (UNLIKELY(key < 0))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADSTATE);
	return (hebi_allocid)key;
}

HEBI_API
hebi_allocid
hebi_alloc_get_default(void)
{
	return getallocalias(HEBI_ALLOC_DEFAULT);
}

HEBI_API
hebi_allocid
hebi_alloc_get_scratch(void)
{
	return getallocalias(HEBI_ALLOC_SCRATCH);
}
