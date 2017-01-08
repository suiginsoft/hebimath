/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

HEBI_API
void
hebi_freefp(const struct hebi_allocfnptrs *fp, void *ptr, size_t size)
{
	(*fp->ha_free)(fp->ha_arg, ptr, size);
}
