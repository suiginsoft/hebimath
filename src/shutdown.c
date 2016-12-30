/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../internal.h"

#ifdef HEBI_DESTRUCTORS

HEBI_DESTRUCTOR
static void
shutdown__(void)
{
	hebi_context_shut__();
	hebi_alloc_table_shut__();
}

#endif /* HEBI_DESTRUCTORS */
