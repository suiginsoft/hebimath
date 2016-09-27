/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

/* TODO: remove these macros after porting to x86-64 assembly */
#ifdef HEBI_MULTI_VERSIONING
#define STATIC static
#ifdef USE_LIMB64_MULDIV
#define PDIVREMRU_3x2_IMPL hebi_pdivremru64_3x2_impl__
#else
#define PDIVREMRU_3x2_IMPL hebi_pdivremru32_3x2_impl__
#endif
#else
#define STATIC
#define PDIVREMRU_3x2_IMPL PDIVREMRU_3x2
#endif

STATIC HEBI_HIDDEN
DLIMB
PDIVREMRU_3x2_IMPL(
		MLIMB *q,
		const MLIMB *a,
		size_t n,
		int bits,
		MLIMB d1,
		MLIMB d0,
		MLIMB v)
{
	MLIMB a1, a0, u2, u1, u0;
	size_t i;

	ASSERT(n > 2);
	ASSERT(MLIMB_BIT > bits && bits >= 0);
	ASSERT((d1 & MLIMB_HIGH_BIT) != 0);

	a1 = a[n-1];
	a0 = a[n-2];

	if (bits) {
		u2 = (a1 << bits) | (a0 >> (MLIMB_BIT - bits));
		u1 = a0 << bits;
		u0 = a1 >> (MLIMB_BIT - bits);
		if ((u2 < d1 || (u2 == d1 && u1 < d0)) && !u0) {
			q[--n] = 0;
			a0 = a[n-2];
			u1 |= a0 >> (MLIMB_BIT - bits);
		} else {
			u1 = u2;
			u2 = u0;
		}
		a1 = a0;
		q[--n] = 0;
		for (i = n-1; i--; ) {
			a0 = a[i];
			u0 = (a1 << bits) | (a0 >> (MLIMB_BIT - bits));
			a1 = a0;
			DIVREMRU_3x2(q+i+1, &u2, &u1, u0, d1, d0, v);
		}
		u0 = a1 << bits;
		DIVREMRU_3x2(q, &u2, &u1, u0, d1, d0, v);
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
		for (i = n - 2; i--; ) {
			u0 = a[i];
			DIVREMRU_3x2(q+i, &u2, &u1, u0, d1, d0, v);
		}
	}

	return (((DLIMB)u2 << MLIMB_BIT) | u1) >> bits;
}

#ifdef HEBI_MULTI_VERSIONING
#ifdef USE_LIMB64_MULDIV
HEBI_HIDDEN DLIMB (*hebi_pdivremru64_3x2_ptr__)(MLIMB*, const MLIMB *, size_t, int, MLIMB, MLIMB, MLIMB) = hebi_pdivremru64_3x2_impl__;
#else
HEBI_HIDDEN DLIMB (*hebi_pdivremru32_3x2_ptr__)(MLIMB*, const MLIMB *, size_t, int, MLIMB, MLIMB, MLIMB) = hebi_pdivremru32_3x2_impl__;
#endif
#endif
