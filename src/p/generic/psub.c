/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_psub(
		hebi_packet *r,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	hebi_word *rw = r->hp_words;
	const hebi_word *aw = a->hp_words;
	const hebi_word *bw = b->hp_words;
	hebi_word difference, borrow;
	size_t i;

	borrow = 0;
	i = 0;

	for ( ; i < bn * HEBI_PACKET_WORDS; ++i) {
		difference = aw[i] - bw[i] - borrow;
		borrow = (difference > aw[i]) || (difference == aw[i] && borrow);
		rw[i] = difference;
	}

	for ( ; i < an * HEBI_PACKET_WORDS; ++i) {
		difference = aw[i] - borrow;
		borrow = difference > aw[i];
		rw[i] = difference;
	}

	return borrow;
}
