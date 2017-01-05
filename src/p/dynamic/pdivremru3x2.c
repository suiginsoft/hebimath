/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../pcommon.h"

extern HEBI_HIDDEN
DLIMB
PDIVREMRU_3x2(MLIMB *restrict q, const MLIMB *restrict a, size_t n,
		unsigned int bits, MLIMB d0, MLIMB d1, MLIMB v);
