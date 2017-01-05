/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../pcommon.h"

extern HEBI_HIDDEN
MLIMB
PDIVREMR(hebi_packet *restrict q, hebi_packet *restrict u,
		const hebi_packet *restrict d, size_t m,
		size_t n, size_t l, MLIMB v);
