/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../../internal.h"

#ifdef HEBI_MULTI_VERSIONING

#define INTERNAL_MVFUNC(R,N,P,A) \
extern R (* N##_ptr__) P; \
static inline HEBI_ALWAYSINLINE \
R N##__ P { return (*N##_ptr__) A; }

#define INTERNAL_MVFUNC_ATTRIB(R,N,P,A,X) \
extern R (* N##_ptr__) P; \
static inline HEBI_ALWAYSINLINE X \
R N##__ P { return (*N##_ptr__) A; }

#else

#define INTERNAL_MVFUNC(R,N,P,A) R N##__ P;
#define INTERNAL_MVFUNC_ATTRIB(R,N,P,A,X) X R N##__ P;

#endif

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
INTERNAL_MVFUNC_ATTRIB(uint64_t, hebi_recipu64_2x1, (uint64_t d), (d), HEBI_CONST)
INTERNAL_MVFUNC_ATTRIB(uint64_t, hebi_recipu64_3x2, (uint64_t d1, uint64_t d0), (d1,d0), HEBI_CONST)
INTERNAL_MVFUNC(uint64_t, hebi_pdivremru64_2x1, (uint64_t *q, const uint64_t *a, size_t n, int b, uint64_t d, uint64_t v), (q,a,n,b,d,v))
INTERNAL_MVFUNC(hebi_uint128, hebi_pdivremru64_3x2, (uint64_t *q, const uint64_t *a, size_t n, int b, uint64_t d1, uint64_t d0, uint64_t v), (q,a,n,b,d1,d0,v))
INTERNAL_MVFUNC(uint64_t, hebi_pdivremr64, (hebi_packet *restrict q, hebi_packet *restrict u, const hebi_packet *restrict d, size_t m, size_t n, size_t l, uint64_t v), (q,u,d,m,n,l,v))

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
INTERNAL_MVFUNC_ATTRIB(uint32_t, hebi_recipu32_2x1, (uint32_t d), (d), HEBI_CONST)
INTERNAL_MVFUNC_ATTRIB(uint32_t, hebi_recipu32_3x2, (uint32_t d1, uint32_t d0), (d1,d0), HEBI_CONST)
INTERNAL_MVFUNC(uint32_t, hebi_pdivremru32_2x1, (uint32_t *q, const uint32_t *a, size_t n, int b, uint32_t d, uint32_t v), (q,a,n,b,d,v))
INTERNAL_MVFUNC(uint64_t, hebi_pdivremru32_3x2, (uint32_t *q, const uint32_t *a, size_t n, int b, uint32_t d1, uint32_t d0, uint32_t v), (q,a,n,b,d1,d0,v))
INTERNAL_MVFUNC(uint32_t, hebi_pdivremr32, (hebi_packet *restrict q, hebi_packet *restrict u, const hebi_packet *restrict d, size_t m, size_t n, size_t l, uint32_t v), (q,u,d,m,n,l,v))

#endif /* USE_LIMB64_MULDIV */
