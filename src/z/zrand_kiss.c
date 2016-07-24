/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_zrand_kiss(
		hebi_zptr restrict r,
		size_t bits,
		struct hebi_kiss *restrict k )
{
	size_t n = (bits + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;

	if (UNLIKELY(!n)) {
		hebi_zsetzero(r);
		return;
	}

	if (n > r->hz_resv)
		hebi_zrealloczero(r, n);

	hebi_prand_kiss(r->hz_packs, n, bits, k);
	r->hz_used = n;
	r->hz_sign = 1;
}
