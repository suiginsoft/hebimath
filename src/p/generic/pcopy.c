/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
hebi_packet *
hebi_pcopy(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n)
{
	(void)memcpy(r, a, n * sizeof(hebi_packet));
	return r + n;
}
