/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "config.h"
#include "hebimath.h"
#include <limits.h>

/* compiler feature detection support */
#if defined __GNUC__ && !defined __clang__

#define GNUC_VERSION_ORDINAL(M,m,p) (((M)*10000)+((m)*100)+(p))
#define GNUC_VERSION GNUC_VERSION_ORDINAL(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#define GNUC_VERSION_AT_LEAST(M,m,p) (GNUC_VERSION >= GNUC_VERSION_ORDINAL(M,m,p))
#define GNUC_VERSION_LESS_THAN(M,m,p) (GNUC_VERSION < GNUC_VERSION_ORDINAL(M,m,p))

#ifndef __has_attribute
#define __has_attribute(X) GNUC_ATTRIBUTE_##X
#define GNUC_ATTRIBUTE_always_inline 1
#define GNUC_ATTRIBUTE_pure 1
#define GNUC_ATTRIBUTE_visibility 1
#endif

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

#ifndef __has_attribute
#define __has_attribute(X) 0
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
EXTENSION typedef unsigned __int128 hebi_uint128;
#endif

#endif /* USE_INT128 */

/* configure preferred limb type for low-level kernel functions */
#ifdef USE_DRIVER_GENERIC

/* detect preferred limb type for most arithmetic/logical operations */
#if !defined USE_LIMB64_ARITHMETIC && !defined USE_LIMB32_ARITHMETIC
#if SIZE_MAX >= UINT64_MAX || defined __LP64__ || defined __ILP32__
#define USE_LIMB64_ARITHMETIC
#else
#define USE_LIMB32_ARITHMETIC
#endif
#endif

#if defined USE_LIMB64_ARITHMETIC && defined USE_LIMB32_ARITHMETIC
#undef USE_LIMB32_ARITHMETIC
#endif

/* detect limb type for full multiplication and division operations */
#ifdef USE_LIMB64_MULDIV
#undef USE_LIMB64_MULDIV
#endif
#ifndef USE_LIMB32_MULDIV
#if defined USE_LIMB64_ARITHMETIC && defined USE_INT128
#define USE_LIMB64_MULDIV
#else
#define USE_LIMB32_MULDIV
#endif
#endif

#if defined USE_LIMB64_MULDIV && defined USE_LIMB32_MULDIV
#undef USE_LIMB32_MULDIV
#endif

#else /* USE_DRIVER_GENERIC */

#undef USE_LIMB64_ARITHMETIC
#undef USE_LIMB32_ARITHMETIC
#undef USE_LIMB64_MULDIV
#undef USE_LIMB32_MULDIV

#if defined USE_DRIVER_X86_64 || defined USE_DRIVER_AARCH64
#define USE_LIMB64_ARITHMETIC
#define USE_LIMB64_MULDIV
#else
#define USE_LIMB32_ARITHMETIC
#define USE_LIMB32_MULDIV
#endif

#endif /* USE_DRIVER_GENERIC */

/* internal-use attributes */
#if __has_attribute(always_inline)
#if __has_attribute(visibility) && defined(__clang__)
#define HEBI_ALWAYSINLINE __attribute__((__always_inline__,__visibility__("hidden")))
#else
#define HEBI_ALWAYSINLINE __attribute__((__always_inline__))
#endif
#else
#define HEBI_ALWAYSINLINE
#endif

#if __has_attribute(visibility)
#define HEBI_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define HEBI_HIDDEN
#endif

#if __has_attribute(pure)
#define HEBI_PURE __attribute__((__pure__))
#else
#define HEBI_PURE
#endif

/* common macros */
#define ABS(A) ((A)<0?-(A):(A))
#define ALIGNAS(A) HEBI_ALIGNAS(A)
#define COUNTOF(A) (sizeof(A) / sizeof(A[0]))
#define MAX(A,B) ((A)>(B)?(A):(B))
#define MIN(A,B) ((A)<(B)?(A):(B))
#define SIGNXOR(A,B) ((int)(((unsigned int)(A))^((unsigned int)(B))))
#define MULTILINEBEGIN do {
#define MULTILINEEND } while(0)
#define LIKELY(E) HEBI_LIKELY(E)
#define UNLIKELY(E) HEBI_UNLIKELY(E)
#define CONCAT__(A,B) A##B
#define CONCAT_(A,B) CONCAT__(A,B)
#define CONCAT(A,B) CONCAT_(A,B)
#define STRINGIZE_(A) #A
#define STRINGIZE(A) STRINGIZE_(A)
#define SWAP(T,A,B) MULTILINEBEGIN { T T_ = A; A = B; B = T_; } MULTILINEEND

/* ignore unused arguments or variables */
HEBI_HIDDEN
int
hebi_ignore__(int x, ...);

#define IGNORE(...) ((void)sizeof(hebi_ignore__(0, __VA_ARGS__)))

/* runtime and static assertions */
HEBI_HIDDEN HEBI_NORETURN
void
hebi_error_assert__(const char *expr, const char *func, const char *file, long line);

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
#define STATIC_ASSERT(E,S) EXTENSION _Static_assert(E, S)
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
#define hk_single hebi_single__
#define hk_multi hebi_multi__
#define hk_length hebi_length__
#define hk_index hebi_index__
#define hk_allocid hebi_allocator__

/* hebi_integer field names */
#define hz_packs hebi_packs__
#define hz_resv hebi_capacity__
#define hz_used hebi_used__
#define hz_sign hebi_sign__
#define hz_allocid hebi_allocator__

#ifdef USE_THREAD_LOCAL

/*
 * the shadow context is for tracking thread-specific resources to be
 * released on thread exit when the main context object is implemented
 * as a TLS variable and doesn't have a destructor of its own
 */
struct hebi_shadow_context {
	/* pointer to active scratch/temporary memory */
	const struct hebi_allocfnptrs *scratchfp;
	void *scratch;
	size_t scratchsize;

#ifdef USE_ASSERTIONS
	/* internal stack counters tracked for debugging purposes */
	unsigned int zstackused;
#endif
};

#endif /* USE_THREAD_LOCAL */

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

#ifdef USE_THREAD_LOCAL

	/* pointer to shadow context, duplicated here for quick access */
	struct hebi_shadow_context *shadow;

#endif

	/* pointer to active scratch/temporary memory */
	const struct hebi_allocfnptrs *scratchfp;
	void *scratch;
	size_t scratchsize;

	/*
	 * stack of temporary integer objects to track and release
	 * if an error is raised
	 */
	unsigned int zstackused;
	hebi_zptr zstack[ZSTACK_MAX_SIZE];

	/* current default & scratch allocator handles for thread */
	hebi_allocid allocids[2];

#ifdef USE_ALLOC_CACHE

	/*
	 * small hash table using linear chaining that provides
	 * a thread-local cache of allocator pointers
	 */
	unsigned int allocused;
	int allockeys[ALLOC_CACHE_MAX_SIZE];
	const struct hebi_allocfnptrs *allocvalues[ALLOC_CACHE_MAX_SIZE];

#endif
};

/* gets or creates the thread-specific context */
#if defined USE_C11_THREAD_LOCAL
EXTENSION extern HEBI_HIDDEN _Thread_local struct hebi_context hebi_context__;
#elif defined USE_GNUC_THREAD_LOCAL
EXTENSION extern HEBI_HIDDEN __thread struct hebi_context hebi_context__;
#elif defined USE_THREADS
HEBI_HIDDEN HEBI_PURE HEBI_WARNUNUSED
struct hebi_context *
hebi_context_get_or_create__(hebi_errhandler handler, void *arg);
#else
extern HEBI_HIDDEN struct hebi_context hebi_context__;
#endif

/* gets a pointer to the thread-specific context */
static inline HEBI_ALWAYSINLINE HEBI_PURE HEBI_WARNUNUSED
struct hebi_context *
hebi_context_get__(void)
{
#if defined USE_THREAD_LOCAL || !defined USE_THREADS
	return &hebi_context__;
#else
	return hebi_context_get_or_create__(NULL, NULL);
#endif
}

#ifdef USE_THREAD_LOCAL

/* gets or creates the thread-specific shadow context */
HEBI_HIDDEN HEBI_PURE HEBI_WARNUNUSED
struct hebi_shadow_context *
hebi_shadow_context_get_or_create__(struct hebi_context *ctx);

/* gets a pointer to thread-specific shadow context */
static inline HEBI_ALWAYSINLINE HEBI_PURE HEBI_WARNUNUSED
struct hebi_shadow_context *
hebi_shadow_context_get__(struct hebi_context *ctx)
{
	if (ctx->shadow)
		return ctx->shadow;
	return hebi_shadow_context_get_or_create__(ctx);
}

#endif /* USE_THREAD_LOCAL */

/*
 * permanently shuts down the global allocator tables, should only be
 * called by the global library destructor in src/context.c
 */
HEBI_HIDDEN
void
hebi_alloc_table_shut__(void);

#ifdef HEBI_MULTI_VERSIONING

/* hwcaps flags for function multi-versioning */
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

/* retreives the hwcaps flags for the current process */
HEBI_HIDDEN HEBI_PURE
unsigned long
hebi_hwcaps__(void);

/* ease-of-use wrapper that raises the HEBI_ENOHWCAPS error */
HEBI_HIDDEN HEBI_NORETURN
void
hebi_hwcaps_fatal__(void);

#endif /* HEBI_MULTI_VERSIONING */

/* ⌈log2(x)⌉ */
static inline HEBI_ALWAYSINLINE HEBI_CONST
size_t
hebi_ceillog2sz__(size_t x)
{
#if __has_builtin(__builtin_clzl)
	return ((size_t)__builtin_clzl(x | 1) ^
		(CHAR_BIT * sizeof(size_t) - 1)) +
		((x & (x - 1)) ? 1u : 0u);
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
hebi_floorlog2sz__(size_t x)
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
unsigned int
hebi_clz32__(uint32_t x)
{
#if __has_builtin(__builtin_clz)
	return (unsigned int)__builtin_clz(x);
#else
	unsigned int r = 0;
	for ( ; !(x & 0x80000000u); x <<= 1, r++) ;
	return r;
#endif
}

/* count trailing-zeros of 32-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
unsigned int
hebi_ctz32__(uint32_t x)
{
#if __has_builtin(__builtin_ctz)
	return (unsigned int)__builtin_ctz(x);
#else
	unsigned int r = 0;
	for ( ; !(x & 1u); x >>= 1, r++) ;
	return r;
#endif
}

/* count leading-zeros of 64-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
unsigned int
hebi_clz64__(uint64_t x)
{
#if __has_builtin(__builtin_clzll)
	return (unsigned int)__builtin_clzll(x);
#else
	unsigned int r = 0;
	for ( ; !(x & 0x8000000000000000ull); x <<= 1, r++) ;
	return r;
#endif
}

/* count trailing-zeros of 64-bit unsigned integer */
static inline HEBI_ALWAYSINLINE HEBI_CONST
unsigned int
hebi_ctz64__(uint64_t x)
{
#if __has_builtin(__builtin_ctzll)
	return (unsigned int)__builtin_ctzll(x);
#else
	unsigned int r = 0;
	for ( ; !(x & 1u); x >>= 1, r++) ;
	return r;
#endif
}

/* 64-bit x 64-bit -> 128-bit unsigned multiply */
static inline HEBI_ALWAYSINLINE
void
hebi_umul128__(
		uint64_t *restrict lo,
		uint64_t *restrict hi,
		uint64_t x,
		uint64_t y )
{
#ifdef USE_INT128
	hebi_uint128 p = (hebi_uint128)x * y;
	*hi = (uint64_t)(p >> 64);
	*lo = (uint64_t)(p & UINT64_MAX);
#else
	uint64_t a = x >> 32;
	uint64_t b = x & UINT32_MAX;
	uint64_t c = y >> 32;
	uint64_t d = y & UINT32_MAX;
	uint64_t ac = a * c;
	uint64_t bc = b * c;
	uint64_t ad = a * d;
	uint64_t bd = b * d;
	uint64_t mid34 = (bd >> 32) + (bc & UINT32_MAX) + (ad & UINT32_MAX);
	*hi = ac + (bc >> 32) + (ad >> 32) + (mid34 >> 32);
	*lo = (mid34 << 32) | (bd & UINT32_MAX);
#endif
}

/* 64-bit x 64-bit + 64-bit -> 128-bit unsigned multiply & addition */
static inline HEBI_ALWAYSINLINE
void
hebi_umad128__(
		uint64_t *restrict lo,
		uint64_t *restrict hi,
		uint64_t x,
		uint64_t y,
		uint64_t z )
{
#ifdef USE_INT128
	hebi_uint128 p = (hebi_uint128)x * y + z;
	*hi = (uint64_t)(p >> 32);
	*lo = (uint64_t)(p & UINT64_MAX);
#else
	hebi_umul128__(lo, hi, x, y);
	*lo += z;
	*hi += *lo < z;
#endif
}

/* reallocates the internal thread-specific scratchpad buffer */
HEBI_HIDDEN HEBI_ALLOC
void *
hebi_realloc_scratch__(struct hebi_context *ctx, size_t n);

/*
 * gets a pointer to the scratchpad buffer, resizing it to the specified
 * size in bytes if necessary
 */
static inline HEBI_ALWAYSINLINE HEBI_ALLOC
void *
hebi_scratch__(size_t n)
{
	struct hebi_context *ctx = hebi_context_get__();
	if (ctx->scratchsize >= n)
		return ctx->scratch;
	return hebi_realloc_scratch__(ctx, n);
}

/*
 * gets a packet pointer to the scratchpad buffer, resizing it to the
 * specified size in packets if necessary
 */
static inline HEBI_ALWAYSINLINE HEBI_ALLOC
hebi_packet *
hebi_pscratch__(size_t n)
{
	if (UNLIKELY(n > HEBI_PACKET_MAXLEN))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADLENGTH);
	return (hebi_packet *)hebi_scratch__(n * sizeof(hebi_packet));
}

/*
 * pushes a integer pointer on the internal stack, ensuring it will be tracked
 * for cleanup in the event of an error being raised
 */
static inline HEBI_ALWAYSINLINE
void
hebi_zinit_push__(hebi_zptr r, hebi_allocid id)
{
	struct hebi_context *ctx = hebi_context_get__();
	if (UNLIKELY(ctx->zstackused >= ZSTACK_MAX_SIZE))
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOSLOTS);
#if defined USE_ASSERTIONS && defined USE_THREAD_LOCAL
	hebi_shadow_context_get__(ctx)->zstackused++;
#endif
	ctx->zstack[ctx->zstackused++] = r;
	if (UNLIKELY(id == HEBI_ALLOC_INVALID))
		id = HEBI_ALLOC_DEFAULT;
	hebi_zinit_allocator(r, id);
}

/*
 * pops the integer off the internal stack, removing it from being tracked for
 * cleanup. integers must be popped in LIFO order.
 */
static inline HEBI_ALWAYSINLINE
void
hebi_zdestroy_pop__(hebi_zptr r)
{
	struct hebi_context *ctx = hebi_context_get__();
	ASSERT(ctx->zstackused > 0 && ctx->zstack[ctx->zstackused - 1] == r);
	ctx->zstack[--ctx->zstackused] = NULL;
#if defined USE_ASSERTIONS && defined USE_THREAD_LOCAL
	hebi_shadow_context_get__(ctx)->zstackused--;
#endif
	hebi_zdestroy(r);
}

/*
 * internal and unsafe versions of hebi_zrealloc and heb_zrealloczero that
 * always assumes the new capacity is greater than hz_used, doen't modify
 * hz_used or hz_sign, and require the old capacity to be passed in as the
 * third argument to avoid a double-read from memory of that value. these
 * functions return a pointer to the newly allocated packet buffer.
 */

HEBI_HIDDEN
hebi_packet *
hebi_zexpand__(hebi_zptr r, size_t n, size_t oldn);

HEBI_HIDDEN
hebi_packet *
hebi_zexpandcopy__(hebi_zptr r, size_t n, size_t oldn);

HEBI_HIDDEN
hebi_packet *
hebi_zexpandcopyif__(hebi_zptr r, size_t n, size_t oldn, int c);

static inline HEBI_ALWAYSINLINE
hebi_packet *
hebi_zgrow__(hebi_zptr r, size_t n)
{
	if (n <= r->hz_resv)
		return r->hz_packs;
	return hebi_zexpand__(r, n, r->hz_resv);
}

static inline HEBI_ALWAYSINLINE
hebi_packet *
hebi_zgrowcopy__(hebi_zptr r, size_t n)
{
	if (n <= r->hz_resv)
		return r->hz_packs;
	return hebi_zexpandcopy__(r, n, r->hz_resv);
}

static inline HEBI_ALWAYSINLINE
hebi_packet *
hebi_zgrowcopyif__(hebi_zptr r, size_t n, int c)
{
	if (n <= r->hz_resv)
		return r->hz_packs;
	return hebi_zexpandcopyif__(r, n, r->hz_resv, c);
}
