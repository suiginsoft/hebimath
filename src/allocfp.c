/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API HEBI_ALLOC HEBI_WARNUNUSED
void *
hebi_allocfp(const struct hebi_allocfnptrs *fp, size_t alignment, size_t size)
{
	void *ptr;

	if (UNLIKELY(!size || (size & (alignment - 1)) ||
				!alignment || (alignment & (alignment - 1))))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);

	ptr = (*fp->ha_alloc)(fp->ha_arg, alignment, size);
	if (UNLIKELY(!ptr))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);

	return ptr;
}
