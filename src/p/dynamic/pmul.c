/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "dynamic.h"

extern HEBI_API
void
hebi_pmul(hebi_packet *restrict r, const hebi_packet *a,
		const hebi_packet *b, size_t, size_t);
