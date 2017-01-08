/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API
void
hebi_pfree(hebi_allocid id, hebi_packet *p, size_t n)
{
	hebi_pfreefp(hebi_alloc_query(NULL, id), p, n);
}
