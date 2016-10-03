/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
void
hebi_pmove(hebi_packet *r, const hebi_packet *a, size_t n)
{
	ASSERT(n > 0);
	(void)memmove(r, a, n * sizeof(hebi_packet));
}
