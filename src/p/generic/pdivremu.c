/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
uint64_t
hebi_pdivremu(hebi_packet *q, const hebi_packet *a, uint64_t b, size_t n)
{
	HALF* ql;
	const HALF* al;
	size_t nl;
	int bits;
	uint64_t d, r;
#ifdef USE_64BIT_DIVISION
	uint64_t v;
#else
	uint32_t v, d1, d0;
#endif

	if (UNLIKELY(!b))
		return 0;

	ql = HALF_PTR(q);
	al = HALF_PTR(a);
	nl = n * HALF_PER_PACKET;

#ifdef USE_64BIT_DIVISION
	bits = hebi_wclz(b);
	d = b << bits;
	v = hebi_recipu64_2x1__(d);
	r = hebi_pdivremru64_2x1__(ql, al, nl, bits, d, v);
#else
	if (b <= UINT32_MAX) {
		bits = hebi_hclz((uint32_t)b);
		d0 = (uint32_t)b << bits;
		v = hebi_recipu32_2x1__(d0);
		r = hebi_pdivremru32_2x1__(ql, al, nl, bits, d0, v);
	} else {
		bits = hebi_wclz(b);
		d = b << bits;
		d0 = (uint32_t)(d & UINT32_MAX);
		d1 = (uint32_t)(d >> 32);
		v = hebi_recipu32_3x2__(d1, d0);
		r = hebi_pdivremru32_3x2__(ql, al, nl, bits, d1, d0, v);
	}
#endif

	return r;
}
