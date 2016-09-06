/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"
#include <stdio.h>

HEBI_NORETURN
void
hebi_error_assert__(
		const char *expr,
		const char *func,
		const char *file,
		long line)
{
	fprintf(stderr, "Assertion failure in %s (%s:%ld)\nExpression: %s\n",
		func, file, line, expr);
	hebi_error_raise(HEBI_ERRDOM_HEBI, HEBI_EASSERTION);
}
