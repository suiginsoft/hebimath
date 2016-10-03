/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "bench.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct hebi_kiss bench_kiss = HEBI_KISS_INIT;

#ifndef __has_builtin
#define __has_builtin(X) 0
#endif

#if defined __clang__ && __has_builtin(__builtin_readcyclecounter)

#define HAS_CYCLE_COUNTER
#define read_cycle_counter __builtin_readcyclecounter

#elif (defined __clang__ || defined __GNUC__) && defined __x86_64__

#define HAS_CYCLE_COUNTER

static inline unsigned long long
read_cycle_counter(void)
{
	unsigned int eax, edx;
	__asm__ __volatile__ ("rdtsc" : "=a"(eax), "=d"(edx));
	return ((unsigned long long)edx << 32) | eax;
}

#endif

static struct timespec start_time = {0, 0};

#ifdef HAS_CYCLE_COUNTER
static unsigned long long start_cycles = 0;
#endif

static double
timespec_elapsed(struct timespec *start, struct timespec *end)
{
	struct timespec e;

	if (end->tv_nsec < start->tv_nsec) {
		e.tv_sec = end->tv_sec - start->tv_sec - 1;
		e.tv_nsec = end->tv_nsec - start->tv_nsec + 1000000000ll;
	} else {
		e.tv_sec = end->tv_sec - start->tv_sec;
		e.tv_nsec = end->tv_nsec - start->tv_nsec;
	}

	return (double)e.tv_sec + (double)e.tv_nsec / 1000000000.0;
}

void
bench_init(int argc, char *argv[], bench_args *p)
{
	unsigned long x;
	int i = 1;

	if (i < argc) {
		x = strtoul(argv[i++], NULL, 0);
		p->iter = x > INT_MAX ? (int)INT_MAX : (int)x;
	}

	if (i < argc) {
		p->an = (size_t)strtoul(argv[i++], NULL, 0);
		p->bn = i < argc ? (size_t)strtoul(argv[i++], NULL, 0) : p->an;
		p->cn = i < argc ? (size_t)strtoul(argv[i++], NULL, 0) : p->bn;
	}

	if (p->bn > p->an)
		p->bn = p->an;

	if (p->cn > p->bn)
		p->cn = p->bn;
}

void
bench_start(void)
{
#ifdef HEBI_MULTI_VERSIONING
	/*
	 * force initialization of internal hwcaps flags from environment
	 * before we start benchmark, so it doesn't affect the result
	 */
	hebi_packet p;
	hebi_pzero(&p, 1);
#endif

	clock_gettime(CLOCK_MONOTONIC, &start_time);

#ifdef HAS_CYCLE_COUNTER
	start_cycles = read_cycle_counter();
#endif
}

void
bench_stop(void)
{
	struct timespec end_time;
	double duration;

#ifdef HAS_CYCLE_COUNTER
	unsigned long long cycles, end_cycles;
	
	end_cycles = read_cycle_counter();
#endif

	clock_gettime(CLOCK_MONOTONIC, &end_time);
	duration = timespec_elapsed(&start_time, &end_time);

#ifdef HAS_CYCLE_COUNTER
	cycles = end_cycles - start_cycles;
	printf("%.8f seconds %19llu cycles\n", duration, cycles);
#else
	printf("%.8f seconds\n", duration);
#endif
}
