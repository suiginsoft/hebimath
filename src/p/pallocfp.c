/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API HEBI_ALLOC HEBI_WARNUNUSED
hebi_packet *
hebi_pallocfp(const struct hebi_allocfnptrs *fp, size_t n)
{
	size_t size;
	void *ptr;

	if (UNLIKELY(!n || n > HEBI_PACKET_MAXLEN))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADLENGTH);

	size = n * sizeof(hebi_packet);
	ptr = (*fp->ha_alloc)(fp->ha_arg, HEBI_PACKET_ALIGNMENT, size);
	if (UNLIKELY(!ptr))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);

	return ptr;
}
