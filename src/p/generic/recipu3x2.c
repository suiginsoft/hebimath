/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_CONST
MLIMB
RECIPU_3x2(MLIMB d1, MLIMB d0)
{
	MLIMB v, p, t1, t0;
	DLIMB t;

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

	t = (DLIMB)v * d0;
	t0 = (MLIMB)(t & MLIMB_MAX);
	t1 = (MLIMB)(t >> MLIMB_BIT);
	p += t1;

	if (p < t1) {
		v--;
		if (p > d1 || (p == d1 && t0 >= d0))
			v--;
	}

	return v;
}
