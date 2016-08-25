/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
hebi_word
hebi_pdivmodru(
		hebi_packet *r,
		const hebi_packet *a,
		hebi_rcpsrcptr HEBI_RESTRICT b,
		size_t n)
{
	hebi_hword *rhw = r->hp_hwords;
	const hebi_hword *ahw = a->hp_hwords;
	hebi_word dividend, remainder;
	size_t i;

	remainder = 0;
	for (i = n * HEBI_PACKET_HWORDS; i--; ) {
		dividend = ahw[i] | (remainder << HEBI_HWORD_BIT);
		rhw[i] = (hebi_hword)(dividend / b);
		remainder = dividend % b;
	}

	return remainder;
}
