/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API HEBI_ALLOC HEBI_WARNUNUSED
void *
hebi_alloc(hebi_allocid id, size_t alignment, size_t size)
{
	return hebi_allocfp(hebi_alloc_query(NULL, id), alignment, size);
}
