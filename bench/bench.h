/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBIMATH_BENCH_H__
#define HEBIMATH_BENCH_H__

#include "../hebimath.h"

#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))

typedef struct {
	int iter;
	size_t an;
	size_t bn;
	size_t cn;
} bench_args;

extern struct hebi_kiss bench_kiss;

void bench_init(int argc, char **argv, bench_args *params);
void bench_start(void);
void bench_stop(void);

#endif
