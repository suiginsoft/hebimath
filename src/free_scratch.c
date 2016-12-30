/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API
void
hebi_free_scratch(void)
{
	(void)hebi_realloc_scratch__(hebi_context_get(), 0);
}
