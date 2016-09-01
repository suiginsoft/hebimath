/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_GENERIC_H__
#define HEBI_GENERIC_H__

#include "../../../internal.h"

#ifdef USE_INT128

#define FULL hebi_uint128
#define HALF uint64_t
#define SFULL hebi_int128
#define SHALF int64_t
#define HALF_CLZ(X) hebi_wclz(X)
#define HALF_CTZ(X) hebi_wctz(X)
#define HALF_PTR(X) (X)->hp_words
#define HALF_BITS 64
#define HALF_MAX UINT64_MAX
#define HALF_PER_PACKET HEBI_PACKET_WORDS

#else /* USE_INT128 */

#define FULL uint64_t
#define HALF uint32_t
#define SFULL int64_t
#define SHALF int32_t
#define HALF_CLZ(X) hebi_hclz(X)
#define HALF_CTZ(X) hebi_hctz(X)
#define HALF_PTR(X) (X)->hp_hwords
#define HALF_BITS 32
#define HALF_MAX UINT32_MAX
#define HALF_PER_PACKET HEBI_PACKET_HWORDS

#endif /* USE_INT128 */

#ifdef USE_64BIT_DIVISION

#define RECIPU_2x1 hebi_recipu64_2x1__
#define RECIPU_3x2 hebi_recipu64_3x2__
#define DIVREMRU_2x1 hebi_divremru64_2x1__
#define DIVREMRU_3x2 hebi_divremru64_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru64_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru64_3x2__

#else /* USE_64BIT_DIVISION */

#define RECIPU_2x1 hebi_recipu32_2x1__
#define RECIPU_3x2 hebi_recipu32_3x2__
#define DIVREMRU_2x1 hebi_divremru32_2x1__
#define DIVREMRU_3x2 hebi_divremru32_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru32_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru32_3x2__

#endif /* USE_64BIT_DIVISION */

static inline HEBI_ALWAYSINLINE
HALF
DIVREMRU_2x1(HALF *qh, HALF u1, HALF u0, HALF d, HALF v)
{
	HALF q1, q0, r;
	FULL q, u;

	u = ((FULL)u1 << HALF_BITS) | u0;
	q = (FULL)u1 * v + u;
	q0 = (HALF)(q & HALF_MAX);
	q1 = (HALF)(q >> HALF_BITS);
	q1 = q1 + 1;
	r = u0 - q1 * d;

	if (r > q0) {
		q1--;
		r += d;
	}

	if (UNLIKELY(r >= d)) {
		q1++;
		r -= d;
	}

	*qh = q1;
	return r;
}

static inline HEBI_ALWAYSINLINE
FULL
DIVREMRU_3x2(HALF *qh, HALF u2, HALF u1, HALF u0, HALF d1, HALF d0, HALF v)
{
	FULL d, q, r, t, u;
	HALF q1, q0;

	u = ((FULL)u2 << HALF_BITS) | u1;
	q = (FULL)u2 * v + u;
	q0 = (HALF)(q & HALF_MAX);
	q1 = (HALF)(q >> HALF_BITS);
	r = ((FULL)(u1 - q1 * d1) << HALF_BITS) | u0;
	t = (FULL)q1 * d0;
	d = ((FULL)d1 << HALF_BITS) | d0;
	r = r - t - d;
	q1 = q1 + 1;

	if ((HALF)(r >> HALF_BITS) >= q0) {
		q1--;
		r += d;
	}

	if (UNLIKELY(r >= d)) {
		q1++;
		r -= d;
	}

	*qh = q1;
	return r;
}

#endif
