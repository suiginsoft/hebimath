/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"
#include <string.h>

HEBI_API
hebi_packet *
hebi_pzero(hebi_packet *r, size_t n)
{
	memset(r, 0, n * sizeof(hebi_packet));
	return r + n;
}
