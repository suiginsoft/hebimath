/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
void
hebi_pcopy(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n)
{
	ASSERT(n > 0);
	(void)memcpy(r, a, n * sizeof(hebi_packet));
}
