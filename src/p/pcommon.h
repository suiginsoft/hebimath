/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

#ifdef USE_LIMB64_ARITHMETIC

#define LIMB uint64_t
#define LIMB_BIT 64
#define LIMB_MAX UINT64_MAX
#define LIMB_PER_PACKET HEBI_PACKET_LIMBS64
#define LIMB_CLZ(X) hebi_clz64__(X)
#define LIMB_CTZ(X) hebi_ctz64__(X)
#define LIMB_PTR(X) (X)->hp_limbs64

#else /* USE_INT128 */

#define LIMB uint32_t
#define LIMB_BIT 32
#define LIMB_MAX UINT32_MAX
#define LIMB_PER_PACKET HEBI_PACKET_LIMBS32
#define LIMB_CLZ(X) hebi_clz32__(X)
#define LIMB_CTZ(X) hebi_ctz32__(X)
#define LIMB_PTR(X) (X)->hp_limbs32

#endif /* USE_INT128 */

#ifdef USE_LIMB64_MULDIV

#define DLIMB hebi_uint128
#define MLIMB uint64_t
#define MLIMB_BIT 64
#define MLIMB_HIGH_BIT (UINT64_C(1)<<63)
#define MLIMB_MAX UINT64_MAX
#define MLIMB_PER_PACKET HEBI_PACKET_LIMBS64
#define MLIMB_CLZ(X) hebi_clz64__(X)
#define MLIMB_PTR(X) (X)->hp_limbs64

#define RECIPU_2x1 hebi_recipu64_2x1__
#define RECIPU_3x2 hebi_recipu64_3x2__
#define DIVREMRU_2x1 hebi_divremru64_2x1__
#define DIVREMRU_3x2 hebi_divremru64_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru64_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru64_3x2__
#define PDIVREMR hebi_pdivremr64__

extern uint16_t hebi_recipu64_v0lut__[256];
HEBI_CONST uint64_t hebi_recipu64_2x1__(uint64_t);
HEBI_CONST uint64_t hebi_recipu64_3x2__(uint64_t, uint64_t);

#ifdef HEBI_MULTI_VERSIONING

extern uint64_t (* hebi_pdivremru64_2x1_ptr__)(uint64_t *, const uint64_t *, size_t, int, uint64_t, uint64_t);
extern hebi_uint128 (* hebi_pdivremru64_3x2_ptr__)(uint64_t *, const uint64_t *, size_t, int, uint64_t, uint64_t, uint64_t);
extern uint64_t (* hebi_pdivremr64_ptr__)(hebi_packet *HEBI_RESTRICT, hebi_packet *HEBI_RESTRICT, const hebi_packet *HEBI_RESTRICT, size_t, size_t, size_t, uint64_t);

static inline HEBI_ALWAYSINLINE
uint64_t
hebi_pdivremru64_2x1__(uint64_t *r, const uint64_t *a, size_t n, int s, uint64_t d, uint64_t v)
{
	return hebi_pdivremru64_2x1_ptr__(r, a, n, s, d, v);
}

static inline HEBI_ALWAYSINLINE
hebi_uint128
hebi_pdivremru64_3x2__(uint64_t *r, const uint64_t *a, size_t n, int s, uint64_t d1, uint64_t d0, uint64_t v)
{
	return hebi_pdivremru64_3x2_ptr__(r, a, n, s, d1, d0, v);
}

static inline HEBI_ALWAYSINLINE
uint64_t
hebi_pdivremr64__(hebi_packet *HEBI_RESTRICT q, hebi_packet *HEBI_RESTRICT u, const hebi_packet *HEBI_RESTRICT d, size_t m, size_t n, size_t l, uint64_t v)
{
	return hebi_pdivremr64_ptr__(q, u, d, m, n, l, v);
}

#else /* HEBI_MULTI_VERSIONING */

uint64_t hebi_pdivremru64_2x1__(uint64_t *, const uint64_t *, size_t, int, uint64_t, uint64_t);
hebi_uint128 hebi_pdivremru64_3x2__(uint64_t *, const uint64_t *, size_t, int, uint64_t, uint64_t, uint64_t);
uint64_t hebi_pdivremr64__(hebi_packet *HEBI_RESTRICT, hebi_packet *HEBI_RESTRICT, const hebi_packet *HEBI_RESTRICT, size_t, size_t, size_t, uint64_t);

#endif /* HEBI_MULTI_VERSIONING */

#else /* USE_LIMB64_MULDIV */

#define DLIMB uint64_t
#define MLIMB uint32_t
#define MLIMB_BIT 32
#define MLIMB_HIGH_BIT (UINT32_C(1)<<31)
#define MLIMB_MAX UINT32_MAX
#define MLIMB_PER_PACKET HEBI_PACKET_LIMBS32
#define MLIMB_CLZ(X) hebi_clz32__(X)
#define MLIMB_PTR(X) (X)->hp_limbs32

#define RECIPU_2x1 hebi_recipu32_2x1__
#define RECIPU_3x2 hebi_recipu32_3x2__
#define DIVREMRU_2x1 hebi_divremru32_2x1__
#define DIVREMRU_3x2 hebi_divremru32_3x2__
#define PDIVREMRU_2x1 hebi_pdivremru32_2x1__
#define PDIVREMRU_3x2 hebi_pdivremru32_3x2__
#define PDIVREMR hebi_pdivremr32__

extern uint16_t hebi_recipu32_v0lut__[512];
HEBI_CONST uint32_t hebi_recipu32_2x1__(uint32_t);
HEBI_CONST uint32_t hebi_recipu32_3x2__(uint32_t, uint32_t);

#ifdef HEBI_MULTI_VERSIONING

extern uint32_t (* hebi_pdivremru32_2x1_ptr__)(uint32_t *, const uint32_t *, size_t, int, uint32_t, uint32_t);
extern uint64_t (* hebi_pdivremru32_3x2_ptr__)(uint32_t *, const uint32_t *, size_t, int, uint32_t, uint32_t, uint32_t);
extern uint32_t (* hebi_pdivremr32_ptr__)(hebi_packet *HEBI_RESTRICT, hebi_packet *HEBI_RESTRICT, const hebi_packet *HEBI_RESTRICT, size_t, size_t, size_t, uint32_t);

static inline HEBI_ALWAYSINLINE
uint32_t
hebi_pdivremru32_2x1__(uint32_t *r, const uint32_t *a, size_t n, int s, uint32_t d, uint32_t v)
{
	return hebi_pdivremru32_2x1_ptr__(r, a, n, s, d, v);
}

static inline HEBI_ALWAYSINLINE
hebi_uint128
hebi_pdivremru32_3x2__(uint32_t *r, const uint32_t *a, size_t n, int s, uint32_t d1, uint32_t d0, uint32_t v)
{
	return hebi_pdivremru32_3x2_ptr__(r, a, n, s, d1, d0, v);
}

static inline HEBI_ALWAYSINLINE
uint32_t
hebi_pdivremr32__(hebi_packet *HEBI_RESTRICT q, hebi_packet *HEBI_RESTRICT u, const hebi_packet *HEBI_RESTRICT d, size_t m, size_t n, size_t l, uint32_t v)
{
	return hebi_pdivremr32_ptr__(q, u, d, m, n, l, v);
}

#else /* HEBI_MULTI_VERSIONING */

uint32_t hebi_pdivremru32_2x1__(uint32_t *, const uint32_t *, size_t, int, uint32_t, uint32_t);
uint64_t hebi_pdivremru32_3x2__(uint32_t *, const uint32_t *, size_t, int, uint32_t, uint32_t d0, uint32_t v);
uint32_t hebi_pdivremr32__(hebi_packet *HEBI_RESTRICT, hebi_packet *HEBI_RESTRICT, const hebi_packet *HEBI_RESTRICT, size_t, size_t, size_t, uint32_t);

#endif /* HEBI_MULTI_VERSIONING */

#endif /* USE_LIMB64_MULDIV */
