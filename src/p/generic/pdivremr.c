/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

/* TODO: remove these macros after porting to x86-64 assembly */
#ifdef HEBI_MULTI_VERSIONING
#define STATIC static
#ifdef USE_LIMB64_MULDIV
#define PDIVREMR_IMPL hebi_pdivremr64_impl__
#else
#define PDIVREMR_IMPL hebi_pdivremr32_impl__
#endif
#else
#define STATIC HEBI_HIDDEN
#define PDIVREMR_IMPL PDIVREMR
#endif

STATIC
MLIMB
PDIVREMR_IMPL(
		hebi_packet *restrict q,
		hebi_packet *restrict u,
		const hebi_packet *restrict d,
		size_t un,
		size_t dn,
		size_t limbs,
		MLIMB v )
{
	MLIMB *restrict ql, *restrict ul;
	const MLIMB *restrict dl;
	MLIMB qh, qhat, p1, p0, u1, u0, c1, c0;
	DLIMB p;
	size_t i, m, n;

	ql = MLIMB_PTR(q);
	ul = MLIMB_PTR(u);
	dl = MLIMB_PTR(d);
	m = un * MLIMB_PER_PACKET;
	n = dn * MLIMB_PER_PACKET;

	ASSERT(n > limbs+2);
	ASSERT((dl[n-1] & MLIMB_HIGH_BIT) != 0);

	n -= 2;
	u1 = ul[m+n+1];
	u0 = ul[m+n];
	qh = 0;

	/*
	 * ensure dividend is less than divisor, adjust dividend and top limb
	 * of quotient if necessary
	 */
	if (UNLIKELY((u1 > dl[n+1] || (u1 == dl[n+1] && u0 >= dl[n])) &&
			hebi_pcmp(u+un, d, dn) >= 0)) {
		(void)hebi_psub(u+un, u+un, d, dn, dn);
		u1 = ul[m+n+1];
		u0 = ul[m+n];
		qh = 1;
	}

	/* perform multi-word 3x2 reciprocal division */
	while (m--) {
		/* estimate quotient limb */
		if (LIKELY(u1 < dl[n+1] || u0 < dl[n])) {
			DIVREMRU_3x2(&qhat, &u1, &u0, ul[m+n], dl[n+1], dl[n], v);
		} else {
			u0 = u0 + dl[n];
			c0 = dl[n] > u0;
			p0 = dl[n] - (dl[n] > 0);
			c1 = p0 - c0;
			u1 = u1 + dl[n+1] - c1;
			qhat = MLIMB_MAX;
		}

		/* multiply and subtract to compute remainder */
		c1 = 0;
		for (i = limbs; i < n; i++) {
			p = (DLIMB)qhat * dl[i];
			p0 = (MLIMB)(p & MLIMB_MAX);
			p1 = (MLIMB)(p >> MLIMB_BIT);
			p0 = p0 + c1;
			c1 = p1 + (p0 < c1);
			c0 = ul[m+i];
			p0 = c0 - p0;
			c1 = c1 + (c0 < p0);
			ul[m+i] = p0;
		}
		c0 = u0 < c1;
		u0 = u0 - c1;
		c1 = u1 < c0;
		u1 = u1 - c0;
		ul[m+n] = u0;

		/* adjust if quotient estimate was too large */
		if (UNLIKELY(c1)) {
			c1 = 0;
			for (i = limbs; i < n+1; i++) {
				c0 = ul[m+i] + dl[i] + c1;
				c1 = (c0 < ul[m+i]) || (c0 == ul[m+i] && c1);
				ul[m+i] = c0;
			}
			u1 = u1 + dl[n+1] + c1;
			qhat--;
		}

		ql[m] = qhat;
	}

	/* store top limb of remainder and return top limb of quotient */
	ul[n+1] = u1;
	return qh;
}

#ifdef HEBI_MULTI_VERSIONING
#ifdef USE_LIMB64_MULDIV
HEBI_HIDDEN MLIMB (*hebi_pdivremr64_ptr__)(hebi_packet *, hebi_packet *, const hebi_packet *, size_t, size_t, size_t, MLIMB) = hebi_pdivremr64_impl__;
#else
HEBI_HIDDEN MLIMB (*hebi_pdivremr32_ptr__)(hebi_packet *, hebi_packet *, const hebi_packet *, size_t, size_t, size_t, MLIMB) = hebi_pdivremr32_impl__;
#endif
#endif
