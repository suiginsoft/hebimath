/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API
void
hebi_free(hebi_allocid id, void *ptr, size_t size)
{
	hebi_freefp(hebi_alloc_query(NULL, id), ptr, size);
}
