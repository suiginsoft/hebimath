/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
size_t
hebi_pshr(hebi_packet *r, const hebi_packet *a, size_t b, size_t n)
{
	size_t i, j, k, rn, words, bits, rbits;
	hebi_word *rw, q, s;
	const hebi_word *aw;

	rn = b / HEBI_PACKET_BIT;
	if (UNLIKELY(n <= rn))
		return 0;

	words = b / HEBI_WORD_BIT;
	bits = b % HEBI_WORD_BIT;
	rbits = HEBI_WORD_BIT - bits;

	rn = n - rn;
	rw = r->hp_words;
	aw = a->hp_words + words;

	if (LIKELY(bits)) {
		q = aw[0] >> bits;
		for (i = 1; i < rn * HEBI_PACKET_WORDS; i++) {
			s = aw[i];
			rw[i-1] = s << rbits | q;
			q = s >> bits;
		}
		rw[i-1] = q;
	} else {
		(void)memmove(rw, aw, rn * sizeof(hebi_packet));
		i = rn * HEBI_PACKET_WORDS;
	}

	if ((j = i & ~(HEBI_PACKET_WORDS - 1)) != i)
	{
		k = i;
		for (words %= HEBI_PACKET_WORDS; words > 0; words--)
			rw[k++] = 0;
	} else {
		j -= HEBI_PACKET_WORDS;
	}

	for ( ; j < i; j++)
		if (rw[j])
			return rn;

	return rn - 1;
}
