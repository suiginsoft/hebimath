/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_INTERNAL_H__
#define HEBI_INTERNAL_H__

#include "config.h"
#include "hebimath.h"
#include <limits.h>

/* compiler feature detection support */
#if defined __GNUC__ && !defined __clang__

#define GNUC_VERSION_ORDINAL(M,m,p) (((M)*10000)+((m)*100)+(p))
#define GNUC_VERSION GNUC_VERSION_ORDINAL(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#define GNUC_VERSION_AT_LEAST(M,m,p) (GNUC_VERSION >= GNUC_VERSION_ORDINAL(M,m,p))
#define GNUC_VERSION_LESS_THAN(M,m,p) (GNUC_VERSION < GNUC_VERSION_ORDINAL(M,m,p))

#ifndef __has_builtin
#define __has_builtin(X) GNUC_BUILTIN_##X
#define GNUC_BUILTIN___builtin_clz GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___builtin_clzl GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___builtin_clzll GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___builtin_ctz GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___builtin_ctzl GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___builtin_ctzll GNUC_VERSION_AT_LEAST(3,4,0)
#define GNUC_BUILTIN___sync_val_compare_and_swap GNUC_VERSION_AT_LEAST(4,1,0)
#define GNUC_BUILTIN___sync_synchronize GNUC_VERSION_AT_LEAST(4,1,0)
#endif

#ifndef __has_extension
#define __has_extension(X) GNUC_EXTENSION_##X
#define GNUC_EXTENSION_c_atomic GNUC_VERSION_AT_LEAST(4,9,0)
#define GNUC_EXTENSION_c_thread_local GNUC_VERSION_AT_LEAST(4,9,0)
#define GNUC_EXTENSION_c_static_assert GNUC_VERSION_AT_LEAST(4,6,0)
#endif

#endif

#ifndef __has_builtin
#define __has_builtin(X) 0
#endif

#ifndef __has_extension
#define __has_extension(X) 0
#endif

#ifdef __GNUC__
#define EXTENSION __extension__
#else
#define EXTENSION
#endif

/* global destructors */
#ifdef USE_GLOBAL_DESTRUCTORS
#ifdef __GNUC__
#define HEBI_DESTRUCTOR __attribute__((__destructor__))
#else
#undef USE_GLOBAL_DESTRUCTORS
#endif
#endif

/* threading feature detection */
#if defined USE_C11_THREADS || defined USE_POSIX_THREADS

#define USE_THREADS

/*
 * Disable unavailable thread-local storage mechanisms.
 *
 * Note that _Thread_local and __thread do not work on Apple
 * or Android platforms currently. Android may be getting
 * support in NDK versions after 11c.
 */
#if !__has_extension(c_thread_local) && ( \
	!defined __STDC_VERSION__ || \
	__STDC_VERSION__ < 201112L || \
	defined __STDC_NO_THREADS__ )
#undef USE_C11_THREAD_LOCAL
#endif

#if !defined USE_C11_THREAD_LOCAL && defined __GNUC__
#if GNUC_VERSION_LESS_THAN(3,3,1)
#undef USE_GNUC_THREAD_LOCAL
#endif
#endif

#if defined __APPLE__ || defined __ANDROID__
#undef USE_C11_THREAD_LOCAL
#undef USE_GNUC_THREAD_LOCAL
#endif

#if defined USE_C11_THREAD_LOCAL || defined USE_GNUC_THREAD_LOCAL
#define USE_THREAD_LOCAL
#endif

/* disable unavailable atomic memory access methods */
#if !__has_extension(c_atomic) && ( \
	!defined __STDC_VERSION__ || \
	__STDC_VERSION__ < 201112L || \
	defined __STDC_NO_ATOMICS__ )
#undef USE_C11_ATOMICS
#endif

#ifdef __has_include
#if !__has_include(<stdatomic.h>)
#undef USE_C11_ATOMICS
#endif
#endif

#if !defined USE_C11_ATOMICS && ( \
	!__has_builtin(__sync_val_compare_and_swap) || \
	!__has_builtin(__sync_synchronize) )
#undef USE_SYNC_ATOMICS
#endif

#if defined USE_C11_ATOMICS || defined USE_SYNC_ATOMICS
#define USE_ATOMICS
#endif

#else /* !defined USE_THREADS */

#undef USE_C11_THREAD_LOCAL
#undef USE_GNUC_THREAD_LOCAL
#undef USE_C11_ATOMICS
#undef USE_SYNC_ATOMICS

#endif /* !defined USE_THREADS */

/* 128-byte integer support */
#ifdef USE_INT128

#if SIZE_MAX < UINT64_MAX && !defined __LP64__ && !defined __ILP32__
#undef USE_INT128
#endif

#ifndef __clang__
#ifdef __GNUC__
#if GNUC_VERSION_LESS_THAN(4,6,0)
#undef USE_INT128
#endif
#else
#undef USE_INT128
#endif
#endif

#ifdef USE_INT128
EXTENSION typedef signed __int128 hebi_int128;
EXTENSION typedef unsigned __int128 hebi_uint128;
#endif

#endif /* USE_INT128 */

/* configure preferred limb type for low-level kernel functions */
#ifdef USE_KERN_GENERIC

/* detect preferred limb type for most arithmetic/logical operations */
#if !defined USE_LIMB64_ARITHMETIC && !defined USE_LIMB32_ARITHMETIC
#if SIZE_MAX >= UINT64_MAX || defined __LP64__ || defined __ILP32__
#define USE_LIMB64_ARITHMETIC
#else
#define USE_LIMB32_ARITHMETIC
#endif
#endif

#ifdef USE_LIMB64_ARITHMETIC
#undef USE_LIMB32_ARITHMETIC
#endif

/* detect limb type for full multiplication and division operations */
#undef USE_LIMB64_MULDIV
#ifndef USE_LIMB32_MULDIV
#if defined USE_LIMB64_ARITHMETIC && defined USE_INT128
#define USE_LIMB64_MULDIV
#else
#define USE_LIMB32_MULDIV
#endif
#endif

#ifdef USE_LIMB64_MULDIV
#undef USE_LIMB32_MULDIV
#endif

#else /* USE_KERN_GENERIC */

#undef USE_LIMB64_ARITHMETIC
#undef USE_LIMB32_ARITHMETIC
#undef USE_LIMB64_MULDIV
#undef USE_LIMB32_MULDIV

#if defined USE_KERN_X86_64 || defined USE_KERN_AARCH64
#define USE_LIMB64_ARITHMETIC
#define USE_LIMB64_MULDIV
#else
#define USE_LIMB32_ARITHMETIC
#define USE_LIMB32_MULDIV
#endif

#endif /* USE_KERN_GENERIC */

/* vector shuffle/swizzle */
#ifdef HEBI_SIMD
#if defined __clang__
#define SHUFFLE2(A,B,X,Y) __builtin_shufflevector((A),(B),(X),(Y))
#define SHUFFLE4(A,B,X,Y,Z,W) __builtin_shufflevector((A),(B),(X),(Y),(Z),(W))
#elif defined __GNUC__
#define SHUFFLE2(A,B,X,Y) __builtin_shuffle((A),(B), EXTENSION (hebi_v2di){(X),(Y)})
#define SHUFFLE4(A,B,X,Y,Z,W) __builtin_shuffle((A),(B), EXTENSION (hebi_v4si){(X),(Y),(Z),(W)})
#endif
#endif

/* common macros */
#define ALIGNAS(A) HEBI_ALIGNAS(A)
#define ABS(A) ((A)<0?-(A):(A))
#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))
#define SWAP(T,A,B) { T T_ = A; A = B; B = T_; }
#define COUNTOF(A) (sizeof(A) / sizeof(A[0]))
#define LIKELY(E) HEBI_LIKELY(E)
#define UNLIKELY(E) HEBI_UNLIKELY(E)
#define CONCAT__(A,B) A##B
#define CONCAT_(A,B) CONCAT__(A,B)
#define CONCAT(A,B) CONCAT_(A,B)
#define STRINGIZE_(A) #A
#define STRINGIZE(A) STRINGIZE_(A)
#define MULTILINEBEGIN do {
#define MULTILINEEND } while(0)

/* ignore unused arguments or variables */
int hebi_ignore__(int, ...);
#define IGNORE(...) ((void)sizeof(hebi_ignore__(0, __VA_ARGS__)))

/* runtime and static assertions */
HEBI_NORETURN void hebi_error_assert__(const char *, const char *, const char *, long);
#ifdef USE_ASSERTIONS
#define ASSERT(E) \
	MULTILINEBEGIN \
	if (UNLIKELY(!(E))) \
		hebi_error_assert__(STRINGIZE(E), __func__, __FILE__, __LINE__); \
	MULTILINEEND
#else
#define ASSERT(E) IGNORE(E)
#endif

#if __has_extension(c_static_assert) || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)
#define STATIC_ASSERT(E,S) _Static_assert(E, S)
#else
#define STATIC_ASSERT(E,S) typedef char CONCAT(static_assert_on_line_, __LINE__)[(!!(E))*2-1]
#endif

/* hebi_errstate field names */
#define hes_handler hebi_handler__
#define hes_arg hebi_arg__
#define hes_longjmp hebi_longjmp__
#define hes_val hebi_val__
#define hes_env hebi_env__

/* hebi_kiss field names */
#define hk_xorshift hebi_xorshift__
#define hk_congruential hebi_congruential__
#define hk_carry hebi_carry__
#define hk_length hebi_length__
#define hk_single hebi_data__.hebi_single__
#define hk_multi hebi_data__.hebi_multi__.hebi_values__
#define hk_multi_index hebi_data__.hebi_multi__.hebi_index__

/* hebi_integer field names */
#define hz_packs hebi_packs__
#define hz_resv hebi_capacity__
#define hz_used hebi_used__
#define hz_sign hebi_sign__
#define hz_allocid hebi_allocator__

/* arguments for longjmp */
struct hebi_longjmparg {
	jmp_buf env;
	int val;
};

/* internal thread-local context */
struct hebi_context {
	/*
	 * error handler callback and data, where errarg may point
	 * to errjmpargs in case hebi_error_jmp was invoked
	 */
	hebi_errhandler errhandler;
	void *errarg;
	struct hebi_longjmparg errjmparg;
	struct hebi_error errlast;

	/* current default & scratch allocator handles for thread */
	hebi_alloc_id allocids[2];

	/* pointer to active scratch/temporary memory */
	size_t scratchsize;
	void *scratch;

	/*
	 * stack of temporary integer objects to track and release
	 * if an error is raised
	 */
	unsigned int zstackused;
	hebi_zptr zstack[ZSTACK_MAX_SIZE];

#ifdef USE_ALLOC_CACHE

	/*
	 * small hash table using linear chaining that provides
	 * a thread-local cache of allocator pointers
	 */
	unsigned int allocused;
	int allockeys[ALLOC_CACHE_MAX_SIZE];
	const struct hebi_alloc_callbacks *allocvalues[ALLOC_CACHE_MAX_SIZE];

#endif /* USE_THREADS */
};

#if defined USE_C11_THREAD_LOCAL
#define hebi_context_get() (&hebi_context__)
EXTENSION extern _Thread_local struct hebi_context hebi_context__;
#elif defined USE_GNUC_THREAD_LOCAL
#define hebi_context_get() (&hebi_context__)
EXTENSION extern __thread struct hebi_context hebi_context__;
#elif defined USE_THREADS
#define hebi_context_get() (hebi_context_get__(NULL, NULL))
HEBI_PURE HEBI_WARNUNUSED struct hebi_context *hebi_context_get__(hebi_errhandler, void *);
#ifdef USE_GLOBAL_DESTRUCTORS
void hebi_context_shutdown__(void);
#endif
#else
#define hebi_context_get() (&hebi_context__)
extern struct hebi_context hebi_context__;
#endif

/* hwcaps detection for multi-versioning */
#ifdef HEBI_MULTI_VERSIONING

enum {
#if defined __i386__ || defined __x86_64__
	hebi_hwcap_sse          = 0x00000001,
	hebi_hwcap_sse2         = 0x00000002,
	hebi_hwcap_sse3         = 0x00000004,
	hebi_hwcap_ssse3        = 0x00000008,
	hebi_hwcap_sse4_1       = 0x00000010,
	hebi_hwcap_sse4_2       = 0x00000020,
	hebi_hwcap_aesni        = 0x00000040,
	hebi_hwcap_clmul        = 0x00000080,
	hebi_hwcap_popcnt       = 0x00000100,
	hebi_hwcap_lzcnt        = 0x00000200,
	hebi_hwcap_f16c         = 0x00000400,
	hebi_hwcap_fma          = 0x00000800,
	hebi_hwcap_avx          = 0x00001000,
	hebi_hwcap_avx2         = 0x00002000,
	hebi_hwcap_bmi          = 0x00004000,
	hebi_hwcap_bmi2         = 0x00008000,
	hebi_hwcap_ermsb        = 0x00010000,
	hebi_hwcap_adx          = 0x00020000,
	hebi_hwcap_sha          = 0x00040000,
	hebi_hwcap_avx512f      = 0x00080000,
	hebi_hwcap_avx512bw     = 0x00100000,
	hebi_hwcap_avx512cd     = 0x00200000,
	hebi_hwcap_avx512dq     = 0x00400000,
	hebi_hwcap_avx512er     = 0x00800000,
	hebi_hwcap_avx512pf     = 0x01000000,
	hebi_hwcap_avx512vl     = 0x02000000,
	hebi_hwcap_avx512ifma   = 0x04000000,
	hebi_hwcap_avx512vbmi   = 0x08000000
#elif defined __arm__ || defined __aarch64__
	hebi_hwcap_neon         = 0x00000001,
	hebi_hwcap_crc          = 0x00000002,
	hebi_hwcap_crypto       = 0x00000004,
	hebi_hwcap_fp16         = 0x00000008,
	hebi_hwcap_rdma         = 0x00000010
#endif
};

HEBI_PURE unsigned long hebi_hwcaps__(void);
HEBI_NORETURN void hebi_hwcaps_fatal__(void);

#endif /* HEBI_MULTI_VERSIONING */

/* ⌈log2(x)⌉ */
static inline HEBI_ALWAYSINLINE HEBI_CONST
size_t
hebi_ceillog2sz(size_t x)
{
#if __has_builtin(__builtin_clzl)
	return ((size_t)__builtin_clzl(x | 1) ^
		(CHAR_BIT * sizeof(size_t) - 1)) +
		(!!(x & (x - 1)));
#else
	size_t r = 0;
	if (LIKELY(x))
		for (x--; x > 0; x >>= 1, r++) ;
	return r;
#endif
}

/* ⌊log2(x)⌋ */
static inline HEBI_ALWAYSINLINE HEBI_CONST
size_t
hebi_floorlog2sz(size_t x)
{
#if __has_builtin(__builtin_clzl)
	return (size_t)__builtin_clzl(x | 1) ^
		(CHAR_BIT * sizeof(size_t) - 1);
#else
	size_t r = 0;
	for ( ; x >>= 1; r++) ;
	return r;
#endif
}

/* count leading-zeros of 32-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
int
hebi_clz32(uint32_t x)
{
#if __has_builtin(__builtin_clz)
	return __builtin_clz(x);
#else
	int r = 0;
	for ( ; !(x & 0x80000000u); x <<= 1, r++) ;
	return r;
#endif
}

/* count trailing-zeros of 32-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
int
hebi_ctz32(uint32_t x)
{
#if __has_builtin(__builtin_ctz)
	return __builtin_ctz(x);
#else
	int r = 0;
	for ( ; !(x & 1u); x >>= 1, r++) ;
	return r;
#endif
}

/* count leading-zeros of 64-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
int
hebi_clz64(uint64_t x)
{
#if __has_builtin(__builtin_clzll)
	return __builtin_clzll(x);
#else
	int r = 0;
	for ( ; !(x & 0x8000000000000000ull); x <<= 1, r++) ;
	return r;
#endif
}

/* count trailing-zeros of 64-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
int
hebi_ctz64(uint64_t x)
{
#if __has_builtin(__builtin_ctzll)
	return __builtin_ctzll(x);
#else
	int r = 0;
	for ( ; !(x & 1u); x >>= 1, r++) ;
	return r;
#endif
}

/* internal integer division kernels */
#ifdef USE_LIMB64_MULDIV

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

/* compare single packet 'a' against UINT_64MAX */
static inline HEBI_ALWAYSINLINE
int
hebi_pcmpgtui64max(const hebi_packet *a)
{
#if defined HEBI_SIMD && defined __SSE2__

	hebi_v4si x;
	int c;

#if !defined __SSE4_1__ || HEBI_PACKET_VECTORS == 1
	hebi_v4si z = EXTENSION (hebi_v4si) { 0, 0, 0, 0 };
#endif

#if HEBI_PACKET_VECTORS == 2

	hebi_v4si m = EXTENSION (hebi_v4si) { 0, 0, UINT32_MAX, UINT32_MAX };
	x = a->hp_v4si[0];
	x = x & m;
	x = x | a->hp_v4si[1];

#ifdef __SSE4_1__
	c = __builtin_ia32_ptestz128((hebi_v2di)x, (hebi_v2di)x);
	return !c;
#else
	x = x == z;
	c = __builtin_ia32_pmovmskb128((hebi_v16qi)x);
	return c != 0xFFFF;
#endif

#else /* HEBI_PACKET_VECTORS == 2 */

	x = a->hp_v4si[0];
	x = x == z;
	c = __builtin_ia32_pmovmskb128((hebi_v16qi)x);
	return (c & 0xFF00) != 0xFF00;

#endif

#else /* HEBI_SIMD */

	int i;

	for (i = 1; i < HEBI_PACKET_LIMBS64; i++)
		if (a->hp_limbs64[i])
			return 1;

	return 0;

#endif /* HEBI_SIMD */
}

/* convert single packet to unsigned 64-bit integer with saturation */
static inline HEBI_ALWAYSINLINE
uint64_t
hebi_pgetsu(const hebi_packet *a)
{
#if defined HEBI_SIMD && defined __SSE2__

#ifdef __SSE4_1__

	hebi_v2di x, y, z, m;
	x = a->hp_v2di[0];
	m = EXTENSION (hebi_v2di) { 0, UINT64_MAX };
	z = EXTENSION (hebi_v2di) { 0, 0 };
	y = x & m;
#if HEBI_PACKET_VECTORS == 2
	y = y | a->hp_v2di[1];
#endif
	z = ~(z == y);
	y = SHUFFLE2(z, z, 1, 1);

#else

	hebi_v4si x, y, z, m;
	x = a->hp_v4si[0];
	m = EXTENSION (hebi_v4si) { 0, 0, UINT32_MAX, UINT32_MAX };
	z = EXTENSION (hebi_v4si) { 0, 0, 0, 0 };
	y = x & m;
#if HEBI_PACKET_VECTORS == 2
	y = y | a->hp_v4si[1];
#endif
	z = ~(z == y);
	y = SHUFFLE4(z, z, 2, 3, 2, 3);
	y = y | z;
	z = SHUFFLE4(y, y, 1, 0, 2, 3);

#endif

	y = y | z;
	x = x | y;

	return ((hebi_v2du)x)[0];

#else /* HEBI_SIMD */

	int i;

	for (i = 1; i < HEBI_PACKET_LIMBS64; i++)
		if (a->hp_limbs64[i])
			return UINT64_MAX;

	return a->hp_limbs64[0];

#endif /* HEBI_SIMD */
}

static inline HEBI_ALWAYSINLINE HEBI_ALLOC HEBI_WARNUNUSED
void *
hebi_scratch(size_t n)
{
	struct hebi_context *ctx = hebi_context_get();
	const struct hebi_alloc_callbacks *cb;
	if (ctx->scratchsize < n) {
		cb = hebi_alloc_query(NULL, ctx->allocids[1]);
		hebi_free_cb(cb, ctx->scratch, ctx->scratchsize);
		ctx->scratch = hebi_alloc_cb(cb, HEBI_PACKET_ALIGNMENT, n);
		ctx->scratchsize = n;
	}
	return ctx->scratch;
}

static inline HEBI_ALWAYSINLINE HEBI_ALLOC HEBI_WARNUNUSED
hebi_packet *
hebi_pscratch(size_t n)
{
	size_t sz = n * sizeof(hebi_packet);
	if (UNLIKELY(sz / sizeof(hebi_packet) != n))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
	return (hebi_packet *)hebi_scratch(sz);
}

static inline HEBI_ALWAYSINLINE
void
hebi_zinit_push__(hebi_zptr r, hebi_alloc_id id)
{
	struct hebi_context *ctx = hebi_context_get();
	if (UNLIKELY(ctx->zstackused >= ZSTACK_MAX_SIZE))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOSLOTS);
	ctx->zstack[ctx->zstackused++] = r;
	if (UNLIKELY(id == HEBI_ALLOC_INVALID))
		id = HEBI_ALLOC_DEFAULT;
	hebi_zinit_allocator(r, id);
}

static inline HEBI_ALWAYSINLINE
void
hebi_zdestroy_pop__(hebi_zptr r)
{
	struct hebi_context *ctx = hebi_context_get();
	ASSERT(ctx->zstackused > 0 && ctx->zstack[ctx->zstackused - 1] == r);
	ctx->zstack[--ctx->zstackused] = NULL;
	hebi_zdestroy(r);
}

static inline HEBI_ALWAYSINLINE
void
hebi_zrealloc_copyif(hebi_zptr r, size_t n, int c)
{
	if (c)
		hebi_zrealloc(r, n);
	else
		hebi_zrealloczero(r, n);
}

static inline HEBI_ALWAYSINLINE
hebi_packet *
hebi_zgrow_copyif(hebi_zptr r, size_t n, int c)
{
	if (n > r->hz_resv)
		hebi_zrealloc_copyif(r, n, c);
	return r->hz_packs;
}

#endif
