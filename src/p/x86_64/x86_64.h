/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../pcommon.h"

/* TODO: remove these when we're done porting division to assembly language */
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
