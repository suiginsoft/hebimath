/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

#ifdef USE_INT128

HEBI_CONST
uint64_t
hebi_recipu64__(uint64_t d)
{
	uint64_t d0, d8, d40, d63;
	uint64_t v1, v2, v3, v4;
	uint64_t e, f1, f0;
	hebi_uint128 f;
	uint32_t v0;

	d0 = -(d & 1);
	d8 = (d >> 55) & 0xFF;
	d40 = (d >> 24) + 1;
	d63 = (d >> 1) - d0;

	v0 = hebi_recipu64_v0lut__[(size_t)d8];
	v1 = (v0 << 11) - ((v0 * v0 * d40) >> 40) - 1;
	v2 = (v1 << 13) + (((1152921504606846976ull - v1 * d40) * v1) >> 47);

	e = ((v2 & d0) >> 1) - v2 * d63;

	v3 = (v2 << 31) + ((uint64_t)((((hebi_uint128)v2) * e) >> 65));

	f = (hebi_uint128)v3 * d;
	f0 = (uint64_t)(f & UINT64_MAX);
	f1 = (uint64_t)(f >> 64);
	f0 = f0 + d;
	f1 = f1 + d + (f0 < d);

	v4 = v3 - f1;

	return v4;
}

#else

HEBI_CONST
uint32_t
hebi_recipu32__(uint32_t d)
{
	uint32_t d0, d9, d21, d31, e;
	uint32_t v0, v1, v2, v3;

	d0 = -(d & 1);
	d9 = (d >> 22) & 0x1FF;
	d21 = (d >> 11) + 1;
	d31 = (d >> 1) - d0;

	v0 = hebi_recipu32_v0lut__[(size_t)d9];
	v1 = (v0 << 4) - ((uint32_t)((((uint64_t)(v0 * v0)) * d21) >> 32)) - 1;

	e = ((v2 & d0) >> 1) - v1 * d31;

	v2 = (v1 << 15) + ((uint32_t)((((uint64_t)v1) * e) >> 33));
	v3 = v2 - (((uint32_t)(((((uint64_t)v2) * d) + d) >> 32)) + d);

	return v3;
}

#endif
