/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zinitn(size_t count, struct hebi_integer r[count])
{
	const hebi_allocid id = hebi_alloc_get_default();
	size_t i;

	for (i = 0; i < count; i++) {
		r[i].hz_packs = NULL;
		r[i].hz_resv = 0;
		r[i].hz_used = 0;
		r[i].hz_sign = 0;
		r[i].hz_allocid = (int)(intptr_t)id;
	}
}
