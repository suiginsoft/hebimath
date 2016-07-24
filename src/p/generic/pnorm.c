/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
size_t
hebi_pnorm(const hebi_packet *p, size_t n)
{
	const hebi_word *pw = p->hp_words;
	size_t i;

	for (i = n * HEBI_PACKET_WORDS; i--; )
		if (pw[i])
			return (i + HEBI_PACKET_WORDS) / HEBI_PACKET_WORDS;

	return 0;
}
