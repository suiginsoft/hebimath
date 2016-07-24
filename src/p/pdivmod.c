/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"
#include <limits.h>

#ifndef HEBI_HAS_DWORD
#define HALF_ARRAY hp_words
#define HALF_BITS (CHAR_BIT * sizeof(hebi_word))
#define HALF_PER_PACKET HEBI_PACKET_WORDS
#else
#define HALF_ARRAY hp_hwords
#define HALF_BITS (CHAR_BIT * sizeof(hebi_hword))
#define HALF_PER_PACKET HEBI_PACKET_HWORDS
#endif

HEBI_API
size_t
hebi_pdivmod(
		hebi_packet *restrict q,
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		size_t *restrict rn,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	hebi_packet *restrict u, *restrict v;
	size_t s, z, m, n;

	/* normalize divisor and dividend into working memory */
	z = hebi_pclz(b, bn);
	s = z % HALF_BITS;
	z = z / HALF_BITS;

	u = w;
	m = hebi_pshl(u, a, s, an) * HALF_PER_PACKET;
	for ( ; m > 0; m--)
		if (u->HALF_ARRAY[m-1])
			break;

	v = w + an + 1;
	n = hebi_pshl(v, b, s, bn) * HALF_PER_PACKET;

	/* perform normalized division */
	m = hebi_pdivmod_norm(q, u, v, m, n - z);

	/* store de-normalized remainder if caller provided space for it */
	if (r) {
		n = hebi_pshr(r, u, HALF_BITS - s, n / HALF_PER_PACKET);
		if (rn)
			*rn = n;
	}

	return m;
}
