/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API HEBI_ALLOC HEBI_WARNUNUSED
hebi_packet *
hebi_palloc(hebi_allocid id, size_t n)
{
	return hebi_pallocfp(hebi_alloc_query(NULL, id), n);
}
