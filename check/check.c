/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "arg.h"
#include "check.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNTOF(X) (long)(sizeof(X) / sizeof((X)[0]))
#define IGNORE(...) ((void)sizeof(ignore(0, __VA_ARGS__)))
extern int ignore(int, ...);

const int64_t check_i64values[] =
{
	0,
	-1,
	1,
	-2,
	2,
	23534,
	-53535,
	INT32_MIN,
	INT32_MAX,
	55634626,
	-35632467,
	INT64_MIN,
	INT64_MAX,
	52253256262636ll,
	-643662367739236ll,
	0xFFFFll,
	-0xFFFFll,
	0x10000ll,
	-0x10000ll,
	0xFFFFFFFFll,
	-0xFFFFFFFFll,
	0x100000000ll,
	-0x100000000ll
};

const long check_num_i64values = COUNTOF(check_i64values);

const uint64_t check_u64values[] =
{
	0,
	1,
	2,
	9,
	53987,
	0x8000,
	0x8001,
	UINT16_MAX,
	0x73B98FB2,
	0x80000000,
	0x80000001,
	UINT32_MAX,
	0x32EF516AF43D503Full,
	0x8000000000000000ull,
	0x8000000000000001ull,
	UINT64_MAX
};

const long check_num_u64values = COUNTOF(check_u64values);

char *argv0;
long check_pass;
long check_iter;
long check_start_iter;
long check_max_iter;
long check_max_perm;
int check_skip_error;
int check_verbose;

static void*
checkalloc(void *ctx, size_t alignment, size_t size)
{
	void *p;
	int e;

	IGNORE(ctx);

	if (alignment < sizeof(void*))
		alignment = sizeof(void*);

	if (size & (alignment - 1)) {
		errno = EINVAL;
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EBADVALUE);
	}

	e = posix_memalign(&p, alignment, size);
	if (e) {
		errno = e;
		hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_ENOMEM);
	}

	memset(p, 0xDC, size);
	return p;
}

static void
checkfree(void *ctx, void *p, size_t size)
{
	IGNORE(ctx, size);

	if (p) {
		memset(p, 0xEF, size);
		free(p);
	}
}

static const struct hebi_allocfnptrs checkfp =
{
	.ha_alloc = checkalloc,
	.ha_free = checkfree,
	.ha_arg = NULL
};
	
static hebi_allocid checkid = HEBI_ALLOC_INVALID;

static void
checkshut(void)
{
	if (checkid != HEBI_ALLOC_INVALID) {
		hebi_alloc_remove(checkid);
		checkid = HEBI_ALLOC_INVALID;
	}
}

void
checkinit(int argc, char *argv[])
{
	ARGBEGIN {
	case 'i':
		check_skip_error = 1;
		break;
	case 's':
		if (argc-- <= 1) {
			perror("missing start iteration");
			abort();
		}
		argv++;
		errno = 0;
		check_start_iter = strtol(*argv, NULL, 0);
		if (check_start_iter <= 0 || errno) {
			perror("invalid start iteration");
			abort();
		}
		break;
	case 'v':
		check_verbose++;
		break;
	} ARGEND
	
	if (argc >= 1) {
		errno = 0;
		check_max_perm = strtol(*argv, NULL, 0);
		if (check_max_perm <= 0 || errno) {
			perror("invalid max number of permutations");
			abort();
		}
	}

	checkid = hebi_alloc_add(&checkfp);
	atexit(checkshut);

	hebi_alloc_set_default(checkid);
}
