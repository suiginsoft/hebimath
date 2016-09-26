/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <string.h>

void
zdirty(hebi_zptr r, ...)
{
	size_t n, u;
	va_list ap;

	if (!r)
		return;

	va_start(ap, r);

	do {
		n = hebi_zcapacity(r);
		u = hebi_zused(r);
		if (n > u)
			memset(r->hebi_packs__ + u, 0xCD,
					(n - u) * sizeof(hebi_packet));
		r = va_arg(ap, hebi_zptr);
	} while(r);

	va_end(ap);
}
