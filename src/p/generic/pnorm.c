/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pnorm(const hebi_packet *a, size_t n)
{
	const LIMB *al;
	size_t i, r;

	al = LIMB_PTR(a);
	r = 0;

	for (i = n * LIMB_PER_PACKET; i--; ) {
		if (al[i]) {
			r = (i + LIMB_PER_PACKET) / LIMB_PER_PACKET;
			break;
		}
	}

	return r;
}
