/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <stdlib.h>
#include <string.h>
#if defined USE_C11_THREADS
#include <threads.h>
#elif defined USE_POSIX_THREADS
#include <pthread.h>
#endif

#ifdef __PCC__

/* TODO: Implement CPUID for PCC */
#if defined __i386__ || defined __x86_64__
#define __get_cpuid_max(X, Y) 0
#define __cpuid(X, A, B, C, D)
#endif

#elif defined __GNUC__

#if defined __i386__ || defined __x86_64__
#include <cpuid.h>
#endif

#endif

struct name_hwcaps {
	const char *name;
	unsigned long caps;
};

static unsigned long hwcaps = 0;

#if defined USE_C11_THREADS
static once_flag hwcaps_once = ONCE_FLAG_INIT;
#elif defined USE_POSIX_THREADS
static pthread_once_t hwcaps_once = PTHREAD_ONCE_INIT;
#else
static int hwcaps_once = 0;
#endif

#if defined __i386__ || defined __x86_64__

#define HAS_HWCAPS 1

static const struct name_hwcaps hwcapsbyname[] =
{
	{          "sse",          hebi_hwcap_sse },
	{         "sse2",         hebi_hwcap_sse2 },
	{         "sse3",         hebi_hwcap_sse3 },
	{        "ssse3",        hebi_hwcap_ssse3 },
	{         "sse4",       hebi_hwcap_sse4_1 | hebi_hwcap_sse4_2 },
	{        "sse41",       hebi_hwcap_sse4_1 },
	{        "sse42",       hebi_hwcap_sse4_2 },
	{       "sse4_1",       hebi_hwcap_sse4_1 },
	{       "sse4_2",       hebi_hwcap_sse4_2 },
	{       "sse4.1",       hebi_hwcap_sse4_1 },
	{       "sse4.2",       hebi_hwcap_sse4_2 },
	{          "aes",        hebi_hwcap_aesni },
	{        "aesni",        hebi_hwcap_aesni },
	{        "clmul",        hebi_hwcap_clmul },
	{        "pcmul",        hebi_hwcap_clmul },
	{     "pcmulqdq",        hebi_hwcap_clmul },
	{       "popcnt",       hebi_hwcap_popcnt },
	{        "lzcnt",        hebi_hwcap_lzcnt },
	{          "abm",       hebi_hwcap_popcnt | hebi_hwcap_lzcnt },
	{          "f16",         hebi_hwcap_f16c },
	{         "f16c",         hebi_hwcap_f16c },
	{          "fma",          hebi_hwcap_fma },
	{         "fma3",          hebi_hwcap_fma },
	{          "avx",          hebi_hwcap_avx },
	{         "avx2",         hebi_hwcap_avx2 },
	{          "bmi",          hebi_hwcap_bmi },
	{         "bmi1",          hebi_hwcap_bmi },
	{         "bmi2",         hebi_hwcap_bmi2 },
	{        "ermsb",        hebi_hwcap_ermsb },
	{          "adx",          hebi_hwcap_adx },
	{          "sha",          hebi_hwcap_sha },
	{      "avx512f",      hebi_hwcap_avx512f },
	{     "avx512bw",     hebi_hwcap_avx512bw },
	{     "avx512cd",     hebi_hwcap_avx512cd },
	{     "avx512dq",     hebi_hwcap_avx512dq },
	{     "avx512er",     hebi_hwcap_avx512er },
	{     "avx512pf",     hebi_hwcap_avx512pf },
	{     "avx512vl",     hebi_hwcap_avx512vl },
	{   "avx512ifma",   hebi_hwcap_avx512ifma },
	{   "avx512vbmi",   hebi_hwcap_avx512vbmi }
};

static
unsigned long native_hwcaps()
{
	unsigned int max_level, max_ext_level;
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
	unsigned long c = 0;

	max_level = __get_cpuid_max(0, NULL);
	if (HEBI_UNLIKELY(max_level < 1))
		return c;

	__cpuid(0x00000001, eax, ebx, ecx, edx);

	if (edx & (1 << 25)) c |= hebi_hwcap_sse;
	if (edx & (1 << 26)) c |= hebi_hwcap_sse2;
	if (ecx & (1 <<  0)) c |= hebi_hwcap_sse3;
	if (ecx & (1 <<  9)) c |= hebi_hwcap_ssse3;
	if (ecx & (1 << 19)) c |= hebi_hwcap_sse4_1;
	if (ecx & (1 << 20)) c |= hebi_hwcap_sse4_2;
	if (ecx & (1 << 25)) c |= hebi_hwcap_aesni;
	if (ecx & (1 <<  1)) c |= hebi_hwcap_clmul;
	if (ecx & (1 << 23)) c |= hebi_hwcap_popcnt;
	if (ecx & (1 << 29)) c |= hebi_hwcap_f16c;
	if (ecx & (1 << 12)) c |= hebi_hwcap_fma;

	if ((ecx & ((1 << 27) | (1 << 28))) == ((1 << 27) | (1 << 28)))
		c |= hebi_hwcap_avx;

	max_ext_level = __get_cpuid_max(0x80000000, NULL);
	if (max_ext_level > 0x80000000) {
		__cpuid(0x80000001, eax, ebx, ecx, edx);
		if (ecx & (1 << 5))
			c |= hebi_hwcap_popcnt | hebi_hwcap_lzcnt;
	}

	if (max_level > 6) {
		__cpuid(0x00000007, eax, ebx, ecx, edx);

		if (ebx & (1 <<  3)) c |= hebi_hwcap_bmi;
		if (ebx & (1 <<  8)) c |= hebi_hwcap_bmi2;
		if (ebx & (1 <<  9)) c |= hebi_hwcap_ermsb;
		if (ebx & (1 << 19)) c |= hebi_hwcap_adx;
		if (ebx & (1 << 29)) c |= hebi_hwcap_sha;

		if (c & hebi_hwcap_avx) {
			if (ebx & (1 <<  5)) c |= hebi_hwcap_avx2;
			if (ebx & (1 << 16)) c |= hebi_hwcap_avx512f;
			if (ebx & (1 << 30)) c |= hebi_hwcap_avx512bw;
			if (ebx & (1 << 28)) c |= hebi_hwcap_avx512cd;
			if (ebx & (1 << 17)) c |= hebi_hwcap_avx512dq;
			if (ebx & (1 << 27)) c |= hebi_hwcap_avx512er;
			if (ebx & (1 << 31)) c |= hebi_hwcap_avx512vl;
			if (ebx & (1 << 21)) c |= hebi_hwcap_avx512ifma;
			if (ecx & (1 << 21)) c |= hebi_hwcap_avx512vbmi;
		}
	}

	return c;
}

#else

#define HAS_HWCAPS 0

#endif

static void
init_hwcaps(void)
{
#if HAS_HWCAPS

	unsigned long caps, mask;
	char *p, *s, *t, *v;
	int i;

	caps = native_hwcaps();

	if ((v = getenv("HEBI_HWCAPS")) && (s = malloc(strlen(v) + 1))) {
		mask = 0;
		t = strtok_r(strcpy(s, v), " \t\v\r\n", &p);
		while (t) {
			for (i = 0; i < COUNTOF(hwcapsbyname); ++i) {
				if (!strcmp(hwcapsbyname[i].name, t)) {
					mask |= hwcapsbyname[i].caps;
					break;
				}
			}
			t = strtok_r(NULL, " \t\v\r\n", &p);
		}
		caps &= mask;
		free(s);
	}
	
	hwcaps = caps;

#else

	hwcaps = 0;

#endif
}

HEBI_HIDDEN HEBI_PURE
unsigned long
hebi_hwcaps__(void)
{
#if defined USE_C11_THREADS
	call_once(&hwcaps_once, init_hwcaps);
#elif defined USE_POSIX_THREADS
	(void)pthread_once(&hwcaps_once, init_hwcaps);
#else
	if (!hwcaps_once) {
		init_hwcaps();
		hwcaps_once = 1;
	}
#endif
	return hwcaps;
}

HEBI_HIDDEN HEBI_NORETURN
void
hebi_hwcaps_fatal__(void)
{
	hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOHWCAPS);
}
