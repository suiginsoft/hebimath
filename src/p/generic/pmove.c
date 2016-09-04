/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
hebi_packet *
hebi_pmove(hebi_packet *r, const hebi_packet *a, size_t n)
{
	(void)memmove(r, a, n * sizeof(hebi_packet));
	return r + n;
}
