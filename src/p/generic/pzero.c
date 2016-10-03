/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
void
hebi_pzero(hebi_packet *r, size_t n)
{
	ASSERT(n > 0);
	(void)memset(r, 0, n * sizeof(hebi_packet));
}
