/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int
hebi_zcmp(hebi_zsrcptr a, hebi_zsrcptr b)
{
	size_t m, n;
	int r, s;

	r = a->hz_sign;
	s = b->hz_sign;

	if (UNLIKELY(!r && !s))
		return 0;
	else if (r != s)
		return r < s ? -1 : 1;

	m = a->hz_used;
	n = b->hz_used;

	if (m < n)
		return -r;
	else if (m > n)
		return r;

	s = hebi_pcmp(a->hz_packs, b->hz_packs, m);
	return r < 0 ? -s : s;
}
