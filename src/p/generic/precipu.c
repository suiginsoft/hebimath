/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

HEBI_API
int
hebi_precipu(hebi_rcpptr r, uint64_t a)
{
	uint64_t d, d0, d8, d40, d63;
	uint64_t v, v1, v2, v3, v4;
	uint32_t v0;
	int bits;

	if (UNLIKELY(!a)) {
		r->hr_dnorm = 0;
		r->hr_vnorm = 0;
		r->hr_shift = 0;
		r->hr_sign = 0;
		return 0;
	}

	bits = hebi_wclz(a);

	d = a << bits;
	d0 = -(d & 1);
	d8 = (d >> 55) & 0xFF;
	d40 = (d >> 24) + 1;
	d63 = (d >> 1) - d0;

	v0 = hebi_precipu_v0lut__[d8];
	v1 = (v0 << 11) - ((d40 * (v0 * v0)) >> 40) - 1;
	v2 = (v1 << 13) + (((1152921504606846976ull - d40 * v1) * v1) >> 47);
	v3 = ((d0 & v2) >> 1) - d63 * v2;

#ifdef USE_INT128
	v4 = (v2 << 31) + (uint64_t)(((hebi_uint128)v3 * v2) >> 33);
	v = v4 - (d + (uint64_t)(d + (((hebi_uint128)d * v4)) >> 32));
#else
	v4 = (v2 << 31) + (((v3 >> 32) * (v2 >> 32)) >> 1);
	v = v4 - (d + ((d >> 32) * (v4 >> 32)) + ((d + (d * v4)) < d));
#endif

	r->hr_dnorm = d;
	r->hr_vnorm = v;
	r->hr_shift = bits;
	r->hr_sign = 1;
	return 1;
}
