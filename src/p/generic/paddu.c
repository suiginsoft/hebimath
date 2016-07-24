/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_paddu(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n)
{
	hebi_word *rw = r->hp_words;
	const hebi_word *aw = a->hp_words;
	hebi_word sum, carry;
	size_t i;

	carry = b;
	for (i = 0; i < n * HEBI_PACKET_WORDS; ++i) {
		sum = aw[i] + carry;
		carry = sum < aw[i];
		rw[i] = sum;
	}

	return carry;
}
