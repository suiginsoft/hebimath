/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_pdivremu(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n)
{
	int bits;
	uint64_t d;
	HALF v;

	if (UNLIKELY(!b))
		return 0;

	bits = hebi_wclz(b);
	d = b << bits;
	v = RECIPU64(d);
	return PDIVREMRU64(r, a, n, bits, d, v);
}
