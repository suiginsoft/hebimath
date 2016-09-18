/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pctz(const hebi_packet *a, size_t n)
{
	const LIMB *al;
	size_t i, r;

	ASSERT(n > 0);

	al = LIMB_PTR(a);
	n *= LIMB_PER_PACKET;
	i = r = 0;

	do {
		if (al[i]) {
			r = LIMB_CTZ(al[i]);
			break;
		}
	} while (++i < n);

	return i * LIMB_BIT + r;
}
