/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_GENERIC_H
#define HEBI_GENERIC_H

#include "../../../internal.h"

#ifndef USE_INT128

#define FULL hebi_dword
#define HALF hebi_word
#define SFULL hebi_int128
#define SHALF int64_t
#define HALF_CLZ(X) hebi_wclz(X)
#define HALF_CTZ(X) hebi_wctz(X)
#define HALF_PTR(X) (X)->hp_words
#define HALF_BITS HEBI_WORD_BIT
#define HALF_MAX HEBI_WORD_MAX
#define HALF_PER_PACKET HEBI_PACKET_WORDS

#else

#define FULL hebi_word
#define HALF hebi_hword
#define SFULL int64_t
#define SHALF int32_t
#define HALF_CLZ(X) hebi_hclz(X)
#define HALF_CTZ(X) hebi_hctz(X)
#define HALF_PTR(X) (X)->hp_hwords
#define HALF_BITS HEBI_HWORD_BIT
#define HALF_MAX HEBI_HWORD_MAX
#define HALF_PER_PACKET HEBI_PACKET_HWORDS

#endif

#endif
