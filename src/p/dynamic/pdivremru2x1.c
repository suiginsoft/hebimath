/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../pcommon.h"

extern HEBI_HIDDEN
MLIMB
PDIVREMRU_2X1(MLIMB *restrict q, const MLIMB *restrict a, size_t n,
		unsigned int bits, MLIMB d, MLIMB v);
