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

static unsigned long hwcaps = 0;

#if defined USE_C11_THREADS
static once_flag hwcaps_once = ONCE_FLAG_INIT;
#elif defined USE_POSIX_THREADS
static pthread_once_t hwcaps_once = PTHREAD_ONCE_INIT;
#else
static int hwcaps_once = 0;
#endif

struct name_hwcaps {
	const char *name;
	unsigned long caps;
};

#if defined __i386__ || defined __x86_64__

#define HAS_HWCAPS 1

static const struct name_hwcaps hwcaps_byname[] =
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

struct hebi_cpuidregs {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};

HEBI_HIDDEN
int
hebi_cpuid__(struct hebi_cpuidregs *out, uint32_t leaf, uint32_t subleaf);

static inline void
setcap(unsigned long *out, unsigned long caps, uint32_t reg, uint32_t mask)
{
	if ((reg & mask) == mask)
		*out |= caps;
}

static unsigned long
nativecaps(void)
{
	struct hebi_cpuidregs r;
	unsigned long c;

	if (UNLIKELY(!hebi_cpuid__(&r, 0x00000001U, 0)))
		return 0;

	c = 0;
	setcap(&c, hebi_hwcap_sse,    r.edx, 1U << 25);
	setcap(&c, hebi_hwcap_sse2,   r.edx, 1U << 26);
	setcap(&c, hebi_hwcap_sse3,   r.ecx, 1U <<  0);
	setcap(&c, hebi_hwcap_ssse3,  r.ecx, 1U <<  9);
	setcap(&c, hebi_hwcap_sse4_1, r.ecx, 1U << 19);
	setcap(&c, hebi_hwcap_sse4_2, r.ecx, 1U << 20);
	setcap(&c, hebi_hwcap_aesni,  r.ecx, 1U << 25);
	setcap(&c, hebi_hwcap_clmul,  r.ecx, 1U <<  1);
	setcap(&c, hebi_hwcap_popcnt, r.ecx, 1U << 23);
	setcap(&c, hebi_hwcap_f16c,   r.ecx, 1U << 29);
	setcap(&c, hebi_hwcap_fma,    r.ecx, 1U << 12);
	setcap(&c, hebi_hwcap_avx,    r.ecx, (1U << 27) | (1U << 28));

	if (hebi_cpuid__(&r, 0x80000001U, 0))
		setcap(&c, hebi_hwcap_popcnt | hebi_hwcap_lzcnt, r.ecx, 1U << 5);

	if (hebi_cpuid__(&r, 0x00000007U, 0)) {
		setcap(&c, hebi_hwcap_bmi,   r.ebx, 1U <<  3);
		setcap(&c, hebi_hwcap_bmi2,  r.ebx, 1U <<  8);
		setcap(&c, hebi_hwcap_ermsb, r.ebx, 1U <<  9);
		setcap(&c, hebi_hwcap_adx,   r.ebx, 1U << 19);
		setcap(&c, hebi_hwcap_sha,   r.ebx, 1U << 29);
		if (c & hebi_hwcap_avx) {
			setcap(&c, hebi_hwcap_avx2,       r.ebx, 1U <<  5);
			setcap(&c, hebi_hwcap_avx512f,    r.ebx, 1U << 16);
			setcap(&c, hebi_hwcap_avx512bw,   r.ebx, 1U << 30);
			setcap(&c, hebi_hwcap_avx512cd,   r.ebx, 1U << 28);
			setcap(&c, hebi_hwcap_avx512dq,   r.ebx, 1U << 17);
			setcap(&c, hebi_hwcap_avx512er,   r.ebx, 1U << 27);
			setcap(&c, hebi_hwcap_avx512vl,   r.ebx, 1U << 31);
			setcap(&c, hebi_hwcap_avx512ifma, r.ebx, 1U << 21);
			setcap(&c, hebi_hwcap_avx512vbmi, r.ecx, 1U << 21);
		}
	}

	return c;
}

#else

#define HAS_HWCAPS 0

#endif

#if HAS_HWCAPS

static inline unsigned long
findcapbyname(const char *name)
{

	size_t i;

	for (i = 0; i < COUNTOF(hwcaps_byname); ++i)
		if (!strcmp(hwcaps_byname[i].name, name))
			return hwcaps_byname[i].caps;

	return 0;
}

static void
initcaps(void)
{
	unsigned long mask;
	char *p;
	char *s;
	char *t;
	char *v;

	hwcaps = nativecaps();

	v = getenv("HEBI_HWCAPS");
	if (!v)
		return;

	s = strdup(v);
	if (!s)
		return;

	t = strtok_r(s, " \t\v\r\n", &p);
	mask = 0;

	while (t) {
		mask |= findcapbyname(t);
		t = strtok_r(NULL, " \t\v\r\n", &p);
	}

	free(s);

	hwcaps &= mask;
}

#else /* HAS_HWCAPS */

static void
initcaps(void)
{
	hwcaps = 0;
}

#endif /* HAS_HWCAPS */

HEBI_HIDDEN HEBI_PURE
unsigned long
hebi_hwcaps__(void)
{
#if defined USE_C11_THREADS
	call_once(&hwcaps_once, &initcaps);
#elif defined USE_POSIX_THREADS
	(void)pthread_once(&hwcaps_once, &initcaps);
#else
	if (!hwcaps_once) {
		initcaps();
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
