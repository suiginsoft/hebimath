/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

/* TODO: remove these macros after porting this to x86-64 assembly */
#ifdef HEBI_MULTI_VERSIONING
#define STATIC static
#ifdef USE_LIMB64_MULDIV
#define PDIVREMRU_3X2_IMPL hebi_pdivremru64_3x2_impl__
#define PDIVREMRU_3X2_PTR hebi_pdivremru64_3x2_ptr__
#else
#define PDIVREMRU_3X2_IMPL hebi_pdivremru32_3x2_impl__
#define PDIVREMRU_3X2_PTR hebi_pdivremru32_3x2_ptr__
#endif
#else
#define STATIC HEBI_HIDDEN
#define PDIVREMRU_3X2_IMPL PDIVREMRU_3X2
#endif

STATIC
DLIMB
PDIVREMRU_3X2_IMPL(
		MLIMB *q,
		const MLIMB *a,
		size_t n,
		unsigned int bits,
		MLIMB d1,
		MLIMB d0,
		MLIMB v)
{
	MLIMB a1;
	MLIMB a0;
	MLIMB u2;
	MLIMB u1;
	MLIMB u0;
	size_t i;

	ASSERT(n >= 2);
	ASSERT(bits < MLIMB_BIT);
	ASSERT((d1 & MLIMB_HIGH_BIT) != 0);

	a1 = a[n-1];
	a0 = a[n-2];

	if (bits) {
		u2 = a1 >> (MLIMB_BIT - bits);
		u1 = (a1 << bits) | (a0 >> (MLIMB_BIT - bits));
		a1 = a0;
		q[n-1] = 0;
		for (i = n - 2; i != 0; i--) {
			a0 = a[i-1];
			u0 = (a1 << bits) | (a0 >> (MLIMB_BIT - bits));
			a1 = a0;
			DIVREMRU_3X2(q+i, &u2, &u1, u0, d1, d0, v);
		}
		u0 = a1 << bits;
		DIVREMRU_3X2(q, &u2, &u1, u0, d1, d0, v);
	} else {
		u2 = a1;
		u1 = a0;
		u0 = 0;
		if (UNLIKELY(u2 > d1 || (u2 == d1 && u1 >= d0))) {
			u2 -= d1 + (u1 < d0);
			u1 -= d0;
			u0 = 1;
		}
		q[n-1] = 0;
		q[n-2] = u0;
		for (i = n - 2; i != 0; i--) {
			u0 = a[i-1];
			DIVREMRU_3X2(q+i-1, &u2, &u1, u0, d1, d0, v);
		}
	}

	return (((DLIMB)u2 << MLIMB_BIT) | u1) >> bits;
}

#ifdef HEBI_MULTI_VERSIONING
HEBI_HIDDEN DLIMB
(*PDIVREMRU_3X2_PTR)(
		MLIMB*,
		const MLIMB *,
		size_t,
		unsigned int,
		MLIMB,
		MLIMB,
		MLIMB) = PDIVREMRU_3X2_IMPL;
#endif
