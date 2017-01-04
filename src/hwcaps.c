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

static inline unsigned long
gethwcapbyname(const char *name)
{
	size_t i;

	for (i = 0; i < COUNTOF(hwcapsbyname); ++i)
		if (!strcmp(hwcapsbyname[i].name, name))
			return hwcapsbyname[i].caps;

	return 0;
}

static inline void
sethwcap(unsigned long *c, unsigned int reg, unsigned int mask,
		unsigned long hwcap)
{
	if ((reg & mask) == mask)
		*c |= hwcap;
}

static unsigned long
nativehwcaps()
{
	unsigned int maxlevel;
	unsigned int maxextlevel;
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
	unsigned long c = 0;

	maxlevel = (unsigned int)__get_cpuid_max(0, NULL);
	if (UNLIKELY(maxlevel < 1))
		return c;

	__cpuid(0x00000001U, eax, ebx, ecx, edx);

	sethwcap(&c, edx, 1U << 25, hebi_hwcap_sse);
	sethwcap(&c, edx, 1U << 26, hebi_hwcap_sse2);
	sethwcap(&c, ecx, 1U <<  0, hebi_hwcap_sse3);
	sethwcap(&c, ecx, 1U <<  9, hebi_hwcap_ssse3);
	sethwcap(&c, ecx, 1U << 19, hebi_hwcap_sse4_1);
	sethwcap(&c, ecx, 1U << 20, hebi_hwcap_sse4_2);
	sethwcap(&c, ecx, 1U << 25, hebi_hwcap_aesni);
	sethwcap(&c, ecx, 1U <<  1, hebi_hwcap_clmul);
	sethwcap(&c, ecx, 1U << 23, hebi_hwcap_popcnt);
	sethwcap(&c, ecx, 1U << 29, hebi_hwcap_f16c);
	sethwcap(&c, ecx, 1U << 12, hebi_hwcap_fma);
	sethwcap(&c, ecx, (1U << 27) | (1U << 28), hebi_hwcap_avx);

	maxextlevel = (unsigned int)__get_cpuid_max(0x80000000U, NULL);

	if (maxextlevel > 0x80000000U) {
		__cpuid(0x80000001U, eax, ebx, ecx, edx);
		sethwcap(&c, ecx, 1U << 5, hebi_hwcap_popcnt | hebi_hwcap_lzcnt);
	}

	if (maxlevel > 6) {
		__cpuid(0x00000007U, eax, ebx, ecx, edx);

		sethwcap(&c, ebx, 1U <<  3, hebi_hwcap_bmi);
		sethwcap(&c, ebx, 1U <<  8, hebi_hwcap_bmi2);
		sethwcap(&c, ebx, 1U <<  9, hebi_hwcap_ermsb);
		sethwcap(&c, ebx, 1U << 19, hebi_hwcap_adx);
		sethwcap(&c, ebx, 1U << 29, hebi_hwcap_sha);

		if (c & hebi_hwcap_avx) {
			sethwcap(&c, ebx, 1U <<  5, hebi_hwcap_avx2);
			sethwcap(&c, ebx, 1U << 16, hebi_hwcap_avx512f);
			sethwcap(&c, ebx, 1U << 30, hebi_hwcap_avx512bw);
			sethwcap(&c, ebx, 1U << 28, hebi_hwcap_avx512cd);
			sethwcap(&c, ebx, 1U << 17, hebi_hwcap_avx512dq);
			sethwcap(&c, ebx, 1U << 27, hebi_hwcap_avx512er);
			sethwcap(&c, ebx, 1U << 31, hebi_hwcap_avx512vl);
			sethwcap(&c, ebx, 1U << 21, hebi_hwcap_avx512ifma);
			sethwcap(&c, ecx, 1U << 21, hebi_hwcap_avx512vbmi);
		}
	}

	return c;
}

#else

#define HAS_HWCAPS 0

#endif

static void
inithwcaps(void)
{
#if HAS_HWCAPS

	unsigned long caps = nativehwcaps();
	unsigned long mask = 0;
	char *p;
	char *s;
	char *t;
	char *v;

	v = getenv("HEBI_HWCAPS");
	if (!v) {
		hwcaps = caps;
		return;
	}

	s = strdup(v);
	if (!s) {
		hwcaps = caps;
		return;
	}

	t = strtok_r(s, " \t\v\r\n", &p);
	while (t) {
		mask |= gethwcapbyname(t);
		t = strtok_r(NULL, " \t\v\r\n", &p);
	}

	free(s);
	hwcaps = caps & mask;

#else

	hwcaps = 0;

#endif
}

HEBI_HIDDEN HEBI_PURE
unsigned long
hebi_hwcaps__(void)
{
#if defined USE_C11_THREADS
	call_once(&hwcaps_once, inithwcaps);
#elif defined USE_POSIX_THREADS
	(void)pthread_once(&hwcaps_once, inithwcaps);
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
