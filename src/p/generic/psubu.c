/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_psubu(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n)
{
	hebi_word *rw = (hebi_word *)r;
	const hebi_word *aw = (const hebi_word *)a;
	hebi_word difference, borrow;
	size_t i;

	borrow = b;
	for (i = 0; i < n * HEBI_PACKET_WORDS; ++i) {
		difference = aw[i] - b;
		b = difference > aw[i];
		rw[i] = difference;
	}

	return borrow;
}
