/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API
void
hebi_pfreefp(const struct hebi_allocfnptrs *fp, hebi_packet *p, size_t n)
{
	ASSERT(n <= HEBI_PACKET_MAXLEN);
	(*fp->ha_free)(fp->ha_arg, p, n * sizeof(hebi_packet));
}
