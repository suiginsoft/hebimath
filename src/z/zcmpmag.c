/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
int
hebi_zcmpmag(hebi_zsrcptr a, hebi_zsrcptr b)
{
	size_t m, n;

	if (UNLIKELY(!a->hz_sign))
		return -!!b->hz_sign;
	else if (UNLIKELY(!b->hz_sign))
		return 1;

	m = a->hz_used;
	n = b->hz_used;

	if (m < n)
		return -1;
	else if (m > n)
		return 1;

	return hebi_pcmp(a->hz_packs, b->hz_packs, m);
}
