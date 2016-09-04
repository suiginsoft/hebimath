/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_GENERIC_H__
#define HEBI_GENERIC_H__

#include "../../../internal.h"

#ifdef USE_LIMB64_ARITHMETIC

#define LIMB uint64_t
#define LIMB_BIT 64
#define LIMB_MAX UINT64_MAX
#define LIMB_PER_PACKET HEBI_PACKET_LIMBS64
#define LIMB_CLZ(X) hebi_clz64(X)
#define LIMB_CTZ(X) hebi_ctz64(X)
#define LIMB_PTR(X) (X)->hp_limbs64

#else /* USE_INT128 */

#define LIMB uint32_t
#define LIMB_BIT 32
#define LIMB_MAX UINT32_MAX
#define LIMB_PER_PACKET HEBI_PACKET_LIMBS32
#define LIMB_CLZ(X) hebi_clz32(X)
#define LIMB_CTZ(X) hebi_ctz32(X)
#define LIMB_PTR(X) (X)->hp_limbs32

#endif /* USE_INT128 */

#ifdef USE_LIMB64_MULDIV

#define DLIMB hebi_uint128
#define MLIMB uint64_t
#define MLIMB_BIT 64
#define MLIMB_MAX UINT64_MAX
#define MLIMB_PER_PACKET HEBI_PACKET_LIMBS64
#define MLIMB_CLZ(X) hebi_clz64(X)
#define MLIMB_PTR(X) (X)->hp_limbs64

#define RECIPU_2x1 hebi_recipu64_2x1__
#define RECIPU_3x2 hebi_recipu64_3x2__
#define DIVREMRU_2x1 hebi_divremru64_2x1__
#define DIVREMRU_3x2 hebi_divremru64_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru64_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru64_3x2__
#define PDIVREMR_3x2 hebi_pdivremr64_3x2__

#else /* USE_LIMB64_MULDIV */

#define DLIMB uint64_t
#define MLIMB uint32_t
#define MLIMB_BIT 32
#define MLIMB_MAX UINT32_MAX
#define MLIMB_PER_PACKET HEBI_PACKET_LIMBS32
#define MLIMB_CLZ(X) hebi_clz32(X)
#define MLIMB_PTR(X) (X)->hp_limbs32

#define RECIPU_2x1 hebi_recipu32_2x1__
#define RECIPU_3x2 hebi_recipu32_3x2__
#define DIVREMRU_2x1 hebi_divremru32_2x1__
#define DIVREMRU_3x2 hebi_divremru32_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru32_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru32_3x2__
#define PDIVREMR_3x2 hebi_pdivremr32_3x2__

#endif /* USE_LIMB64_MULDIV */

static inline HEBI_ALWAYSINLINE
void
DIVREMRU_2x1(MLIMB *q, MLIMB *u1, MLIMB u0, MLIMB d, MLIMB v)
{
	DLIMB p;
	MLIMB q1, q0, r;

	p = ((DLIMB)*u1 * v) + (((DLIMB)*u1 << MLIMB_BIT) | u0);
	q0 = (MLIMB)(p & MLIMB_MAX);
	q1 = (MLIMB)(p >> MLIMB_BIT);
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

	*q = q1;
	*u1 = r;
}

static inline HEBI_ALWAYSINLINE
void
DIVREMRU_3x2(MLIMB *q, MLIMB *u2, MLIMB *u1, MLIMB u0, MLIMB d1, MLIMB d0, MLIMB v)
{
	DLIMB d, p, r, t;
	MLIMB q1, q0;

	p = ((DLIMB)*u2 * v) + (((DLIMB)*u2 << MLIMB_BIT) | *u1);
	q0 = (MLIMB)(p & MLIMB_MAX);
	q1 = (MLIMB)(p >> MLIMB_BIT);
	r = ((DLIMB)(*u1 - q1 * d1) << MLIMB_BIT) | u0;
	t = (DLIMB)q1 * d0;
	d = ((DLIMB)d1 << MLIMB_BIT) | d0;
	r = r - t - d;
	q1 = q1 + 1;

	if ((MLIMB)(r >> MLIMB_BIT) >= q0) {
		q1--;
		r += d;
	}

	if (UNLIKELY(r >= d)) {
		q1++;
		r -= d;
	}

	*q = q1;
	*u1 = (MLIMB)(r & MLIMB_MAX);
	*u2 = (MLIMB)(r >> MLIMB_BIT);
}

#endif
