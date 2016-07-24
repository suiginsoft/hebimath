/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_padd(
		hebi_packet *r,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	hebi_word *rw = r->hp_words;
	const hebi_word *aw = a->hp_words;
	const hebi_word *bw = b->hp_words;
	hebi_word sum, carry;
	size_t i;

	carry = 0;
	i = 0;

	for ( ; i < bn * HEBI_PACKET_WORDS; ++i) {
		sum = aw[i] + bw[i] + carry;
		carry = (sum < aw[i]) || (sum == aw[i] && carry);
		rw[i] = sum;
	}

	for ( ; i < an * HEBI_PACKET_WORDS; ++i) {
		sum = aw[i] + carry;
		carry = sum < aw[i];
		rw[i] = sum;
	}

	return carry;
}
