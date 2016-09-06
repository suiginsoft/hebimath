/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBI_CONFIG_H__
#define HEBI_CONFIG_H__

/*
 * Uncomment to enable assertions on input argument constraints of low-
 * level kernel functions and internal consistency checks. Assertions
 * guard against programming errors and hence are disabled by default
 * and should only be enabled as an aid when debugging.
 */
/* #define USE_ASSERTIONS */

/*
 * Comment out to disable validation of input arguments on high-level
 * integer functions (functions with hebi_z*** prefix). Validation
 * checks help guard against constraint violations in external data.
 * Disabling this may improve performance at the cost of safety.
 */
#define USE_VALIDATION

/*
 * Enables internal use of __int128 compiler extension when supported
 * by the compiler and target platform.
 */
#define USE_INT128

/*
 * Enables use of compiler/platform specific global destructors for static
 * and shared library cleanup. Disabling this may cause global memory
 * blocks and thread-synchronization resources not to be cleaned up
 * on process exit, deferring to the OS to recover process resources.
 * If you want your programs to be VALGRIND-clean, leave this enabled.
 */
#define USE_GLOBAL_DESTRUCTORS

/*
 * Select the function to use for aligned memory allocation. If no
 * function is selected here, implement aligned allocation on top of
 * malloc and free by allocating an over-sized buffer.
 *
 *     USE_C11_ALIGNED_ALLOC
 *         aligned_alloc from C11 libc
 *
 *     USE_POSIX_MEMALIGN
 *         posix_memalign from POSIX.1-2001
 */
/* #define USE_C11_ALIGNED_ALLOC */
#define USE_POSIX_MEMALIGN

/*
 * Select the threading library to use for thread-safety. When more than
 * one are enabled, the the library that is selected is the first one as
 * listed in the order of precedence below. Commenting out all of the
 * definitions disables thread-safety, in which case hebimath can only be
 * used in a single-threaded environment. Disabling thread-safety may
 * yield some performance benefits for single-threaded use cases.
 *
 *     USE_C11_THREADS
 *         portable C11 libc threads (threads.h)
 *
 *     USE_POSIX_THREADS
 *         pthreads library (pthreads.h)
 */
/* #define USE_C11_THREADS */
#define USE_POSIX_THREADS

/*
 * Select the thread-local storage (TLS) mechanism to use. Note that each of
 * these are hints and only when the compiler and target platform support
 * them are they actually used. When more than one are enabled, the
 * mechanism that is selected is the first one available as listed in order
 * of precedence below. Commenting out all of the definitions will cause
 * hebimath to fallback on using a possibly slower library primitive for
 * thread-local storage provided by the selected threading library above
 * if thread-safety is required.
 *
 *     USE_C11_THREAD_LOCAL
 *         _Thread_local / thread_local keyword from C11
 *
 *     USE_GNUC_THREAD_LOCAL
 *         GNU C TLS using __thread keyword
 */
#define USE_C11_THREAD_LOCAL
#define USE_GNUC_THREAD_LOCAL

/*
 * Select and enable support for different atomic memory access backends.
 * Note that each of these are hints and only when the compiler and
 * target platform support them are they actually used. When more than
 * one are enabled, the backend that is selected is the first one
 * available as listed in order of precedence below. Commenting out all
 * of the atomics definitions disables direct usage of atomic memory
 * access, in which case mutex-based locking may be used if thread-safety
 * is required.
 *
 *     USE_C11_ATOMICS
 *         _Atomic keyword and stdatomic.h header from C11
 *
 *     USE_SYNC_ATOMICS
 *         use Intel C __sync_*** style of atomic builtins
 */
#define USE_C11_ATOMICS
#define USE_SYNC_ATOMICS

/*
 * Specify the maximum cache-line size for target CPU hardware.
 * Refer to your compiler and platform documentation.
 *
 * This value must be a power of 2.
 */
#if defined __i386__ || defined __x86_64__
#define CACHE_LINE_SIZE 64
#elif defined __arm__
#define CACHE_LINE_SIZE 32
#elif defined __aarch64__
#define CACHE_LINE_SIZE 128
#else
#define CACHE_LINE_SIZE 256
#endif

/*
 * Controls if the allocator table is dynamically expanded up to the
 * the total number of allowed allocators as controlled by the
 * MAX_PAGES & PAGE_SIZE definitions below. Commenting this out will
 * use a static fixed-sized array, which may be more suitable for
 * embedded use cases.
 */
#define ALLOC_TABLE_DYNAMIC

/*
 * The following definitions control the maximum size of the internal
 * allocator object table. This limits the total allowed number of
 * allocators to (MAX_PAGES * PAGE_SIZE) % MAX_ALLOCATORS where
 * MAX_ALLOCATORS is the number of unique allocator handles that can
 * be stored in a hebi_integer. On 32-bit platforms MAX_ALLOCATORS is
 * 1024, on 64-bit platforms, MAX_ALLOCATORS is 65536.
 *
 * These values must be powers of 2.
 */
#define ALLOC_TABLE_MAX_PAGES 128
#define ALLOC_TABLE_PAGE_SIZE 32

/*
 * Determines if a thread local cache of allocator callback pointers
 * into the global allocator table is maintained for each thread.
 * Enabling this improves scalability at the cost of additional
 * complexity when resolving allocator ids.
 */
#define USE_ALLOC_CACHE

/*
 * Maximum size and load-factor adjusted size of the thread-local
 * allocator cache, a linearly-addressed hash table. MAX_USED
 * determines the total* number of allocator slots that can be used
 * in cache before it resets itself, whereas MAX_SIZE controls
 * total number of slots and slot distribution.
 *
 * MAX_SIZE must be a power of 2, and MAX_USED < MAX_SIZE.
 */
#define ALLOC_CACHE_MAX_SIZE 32
#define ALLOC_CACHE_MAX_USED 21

/*
 * Total size of thread-local stack that tracks integer object temporaries.
 */
#define ZSTACK_MAX_SIZE 4

/*
 * Cuttoff values in number of packets before Karatsuba multiplication
 * and squaring are used. Cutoff values must be greater or equal to 4.
 */
#define KARATSUBA_MUL_CUTOFF 8
#define KARATSUBA_SQR_CUTOFF 8

#endif
