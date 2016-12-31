/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

HEBI_API
void
hebi_znot(hebi_zptr r, hebi_zsrcptr a)
{
	hebi_zaddu(r, a, 1);
	hebi_zneg(r, r);
}
