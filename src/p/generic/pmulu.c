/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_pmulu(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n)
{
	hebi_word o = 0;
	size_t i;

#ifdef HEBI_HAS_DWORD

	hebi_word *rw = r->hp_words;
	const hebi_word *aw = a->hp_words;
	hebi_dword p;

	for (i = 0; i < n * HEBI_PACKET_WORDS; ++i) {
		p = (hebi_dword)aw[i] * b + o;
		rw[i] = (hebi_word)(p & HEBI_WORD_MAX);
		o = (hebi_word)(p >> HEBI_WORD_BIT);
	}

#else

	hebi_hword *rhw = r->hp_hwords;
	const hebi_hword *ahw = a->hp_hwords;
	const hebi_hword b_lo = (hebi_hword)(b & HEBI_HWORD_MAX);
	const hebi_hword b_hi = (hebi_hword)(b >> HEBI_HWORD_BIT);
	hebi_word p_lo, p_hi;

	for (i = 0; i < n * HEBI_PACKET_HWORDS; ++i) {
		p_lo = (hebi_word)ahw[i] * b_lo + (o & HEBI_HWORD_MAX);
		p_hi = (hebi_word)ahw[i] * b_hi + (o >> HEBI_HWORD_BIT);
		rhw[i] = (hebi_hword)(p_lo & HEBI_HWORD_MAX);
		o = (p_lo >> HEBI_HWORD_BIT) + p_hi;
	}

#endif

	return o;
}
