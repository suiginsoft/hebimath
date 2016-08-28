/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_GENERIC_H__
#define HEBI_GENERIC_H__

#include "../../../internal.h"

#ifdef USE_INT128

#define FULL hebi_uint128
#define HALF uint64_t
#define SFULL hebi_int128
#define SHALF int64_t
#define HALF_CLZ(X) hebi_wclz(X)
#define HALF_CTZ(X) hebi_wctz(X)
#define HALF_PTR(X) (X)->hp_words
#define HALF_BITS 64
#define HALF_MAX UINT64_MAX
#define HALF_PER_PACKET HEBI_PACKET_WORDS

#else /* USE_INT128 */

#define FULL uint64_t
#define HALF uint32_t
#define SFULL int64_t
#define SHALF int32_t
#define HALF_CLZ(X) hebi_hclz(X)
#define HALF_CTZ(X) hebi_hctz(X)
#define HALF_PTR(X) (X)->hp_hwords
#define HALF_BITS 32
#define HALF_MAX UINT32_MAX
#define HALF_PER_PACKET HEBI_PACKET_HWORDS

#endif /* USE_INT128 */

#ifdef USE_64BIT_DIVISION

#define RECIPU hebi_recipu64__
#define RECIPUX2 hebi_recipu64x2__
#define PDIVREMRU hebi_pdivremru64__
#define PDIVREMRUX2 hebi_pdivremru64x2__

#define RECIPU64(D) hebi_recipu64__(D)
#define PDIVREMRU64(R,A,N,S,D,V) hebi_pdivremru64__((R),(A),(N),(S),(D),(V))

#else /* USE_64BIT_DIVISION */

#define RECIPU hebi_recipu32__
#define RECIPUX2 hebi_recipu32x2__
#define PDIVREMRU hebi_pdivremru32__
#define PDIVREMRUX2 hebi_pdivremru32x2__

#define RECIPU64(D) \
hebi_recipu32x2__((HALF)((D)>>HALF_BITS),(HALF)((D)&HALF_MAX))

#define PDIVREMRU64(R,A,N,S,D,V) \
hebi_pdivremru32x2__((R),(A),(N),(S),(HALF)((D)>>HALF_BITS),(HALF)((D)&HALF_MAX),V)

#endif /* USE_64BIT_DIVISION */

#endif
