/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
int
hebi_pcmp(const hebi_packet *a, const hebi_packet *b, size_t n)
{
	const hebi_word *aw = a->hp_words;
	const hebi_word *bw = b->hp_words;
	size_t i;

	for (i = n * HEBI_PACKET_WORDS; i--; ) {
		if (aw[i] < bw[i])
			return -1;
		else if (aw[i] > bw[i])
			return 1;
	}

	return 0;
}
