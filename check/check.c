/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "arg.h"
#include "check.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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

const long check_num_i64values =
	(long)(sizeof(check_i64values) / sizeof(int64_t));

char *argv0;
long check_iter;
long check_max_iter;
long check_max_perm;
int check_skip_error;
int check_verbose;

void
checkinit(int argc, char *argv[])
{
	ARGBEGIN {
	case 's':
		check_skip_error = 1;
		break;
	case 'v':
		check_verbose++;
		break;
	} ARGEND
	
	if (argc >= 1) {
		errno = 0;
		check_max_perm = strtol(*argv++, NULL, 0);
		if (check_max_perm <= 0 || errno) {
			perror("invalid max number of permutations");
			abort();
		}
	}
}
