/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_CONST
HALF
RECIPU_3x2(HALF d1, HALF d0)
{
	HALF v, p, t1, t0;
	FULL t;

	v = RECIPU_2x1(d1);
	p = v * d1 + d0;

	if (p < d0) {
		v--;
		if (p >= d1) {
			v--;
			p -= d1;
		}
		p -= d1;
	}

	t = (FULL)v * d0;
	t0 = (HALF)(t & HALF_MAX);
	t1 = (HALF)(t >> HALF_BITS);
	p += t1;

	if (p < t1) {
		v--;
		if (p > d1 || (p == d1 && t0 >= d0))
			v--;
	}

	return v;
}
