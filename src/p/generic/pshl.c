/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
size_t
hebi_pshl(hebi_packet *r, const hebi_packet *a, size_t b, size_t n)
{
	size_t i, j, rn, words, pwords, bits, rbits;
	hebi_word *rw, q, s;
	const hebi_word *aw;

	if (UNLIKELY(!n))
		return 0;

	words = b / HEBI_WORD_BIT;
	bits = b % HEBI_WORD_BIT;
	pwords = words % HEBI_PACKET_WORDS;
	rbits = HEBI_WORD_BIT - bits;

	aw = a->hp_words;
	rw = r->hp_words + words;
	rn = n + (b + HEBI_PACKET_BIT - 1) / HEBI_PACKET_BIT;

	i = rn * HEBI_PACKET_WORDS - words;
	while (i > n * HEBI_PACKET_WORDS)
		rw[--i] = 0;

	if (LIKELY(bits)) {
		q = 0;
		j = i - pwords - 1;
		while (i > j) {
			s = aw[--i];
			rw[i+1] = s >> rbits | q;
			q = s << bits;
			if (rw[i+1])
				goto shift_rest;
		}
		--rn;
	shift_rest:
		while (i > 0) {
			s = aw[--i];
			rw[i+1] = s >> rbits | q;
			q = s << bits;
		}
		rw[0] = q;
	} else {
		j = i - (pwords ? pwords : HEBI_PACKET_WORDS);
		while (i > j) {
			s = aw[--i];
			rw[i] = s;
			if (s)
				goto move_rest;
		}
		--rn;
	move_rest:
		(void)memmove(rw, aw, i * sizeof(hebi_word));
	}

	(void)memset(r, 0, words * sizeof(hebi_word));
	return rn;
}
