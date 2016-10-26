/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#ifndef HEBIMATH_CHECK_H__
#define HEBIMATH_CHECK_H__

#include "../config.h"
#include "../hebimath.h"
#include <assert.h>
#include <stdarg.h>

#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))

enum {
	LHS_NONZERO = 0x01,
	RHS_NONZERO = 0x02,
	NONZERO = 0x03
};

extern const int64_t check_i64values[];
extern const long check_num_i64values;

extern const uint64_t check_u64values[];
extern const long check_num_u64values;

extern long check_pass;
extern long check_iter;
extern long check_first_iter;
extern long check_max_iter;
extern long check_max_perm;
extern long check_scale_perm;
extern int check_skip_error;
extern int check_verbose;

void checkinit(int argc, char *argv[]);
long maxpermutations(long n, int p);

int vsnchkprintf(char *restrict s, size_t n, const char *restrict fmt, va_list ap);
int vaschkprintf(char **restrict strp, const char *restrict fmt, va_list ap);
int aschkprintf(char **restrict strp, const char *restrict fmt, ...);

void bcwrite(const char *str);
char *bcreadln(void);
char *bcputs(const char *str);
char *bcprintf(const char *format, ...);
char *vbcprintf(const char *format, va_list ap);
void zcheckbc(hebi_zsrcptr restrict actual, const char *bcopfmt, ...);
void zcheckstr(hebi_zsrcptr restrict actual, const char *expected, const char* operation);

void zdirty(hebi_zptr, ...);
void zpermutation(long x, long n, int p, ...);
void zcheckbinop(void (*f)(hebi_zptr, hebi_zsrcptr, hebi_zsrcptr), const char* op, int flags);
void zcheckbinopi64(void (*f)(hebi_zptr, hebi_zsrcptr, int64_t), const char* op, int flags);
void zcheckbinopu64(void (*f)(hebi_zptr, hebi_zsrcptr, uint64_t), const char* op, int flags);
void zcheckshiftop(void (*f)(hebi_zptr, hebi_zsrcptr, size_t), const char* op, int flags);
void zcheckunaryop(void (*f)(hebi_zptr, hebi_zsrcptr), const char* op, int flags);

#endif
