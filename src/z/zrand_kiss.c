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
	hebi_packet *p;
	size_t n;
	
	n = (bits + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;
	if (UNLIKELY(!n)) {
		hebi_zsetzero(r);
		return;
	}

	p = hebi_zgrow__(r, n);
	hebi_prand_kiss(p, n, bits, k);
	r->hz_used = n;
	r->hz_sign = 1;
}
