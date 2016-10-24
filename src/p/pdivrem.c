/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

HEBI_API
size_t
hebi_pdivrem(
		hebi_packet *restrict q,
		hebi_packet *restrict r,
		hebi_packet *restrict w,
		size_t *restrict rn,
		const hebi_packet *a,
		const hebi_packet *b,
		size_t an,
		size_t bn )
{
	size_t bits, limbs, m, n;
	MLIMB d1, d0, v;

	ASSERT(an >= bn);
	ASSERT(bn > 0);

	/* determine number of leading zero bits and size of divisor */
	bits = hebi_pclz(b, bn) % HEBI_PACKET_BIT;
	limbs = bits / MLIMB_BIT;
	n = bn * MLIMB_PER_PACKET - limbs;

	ASSERT(n != 0);

	if (n > 2) {
		/* division with large divisor */
		hebi_packet *restrict u, *restrict d;
		const MLIMB *restrict dl;
		MLIMB qh;

		m = hebi_pshl((u = w), a, bits, an);
		n = hebi_pshl((d = w + m), b, bits, bn);

		ASSERT(m >= n);
		m -= n;

		dl = MLIMB_PTR(d+n)-2;
		d1 = dl[1];
		d0 = dl[0];

		v = RECIPU_3x2(d1, d0);
		if (UNLIKELY((qh = PDIVREMR(q, u, d, m, n, limbs, v))))
			hebi_psetu(q+m++, qh);

		if (r || rn) {
			n = hebi_pnorm(u, n);
			n = hebi_pshr(r ? r : u, u, bits, n);
		}
	} else {
		/* division with small divisor */
		MLIMB *restrict ql;
		const MLIMB *al;
		const MLIMB *bl;
		int shft;

		m = an;
		an = an * MLIMB_PER_PACKET;
		shft = (int)(bits % MLIMB_BIT);

		ql = MLIMB_PTR(q);
		al = MLIMB_PTR(a);
		bl = MLIMB_PTR(b);
		d0 = bl[0];

		if (n == 2) {
			/* 3x2 division by normalized reciprocal */
			DLIMB u;

			d1 = bl[1];
			if (shft) {
				d1 = (d1 << shft) | (d0 >> (MLIMB_BIT - shft));
				d0 = d0 << shft;
			}

			v = RECIPU_3x2(d1, d0);
			u = PDIVREMRU_3x2(ql, al, an, shft, d1, d0, v);

			/* unpack and store remainder */
#ifdef USE_LIMB64_MULDIV
			d0 = (MLIMB)(u & MLIMB_MAX);
			d1 = (MLIMB)(u >> MLIMB_BIT);
			if ((n = (d0 || d1)) && r)
				hebi_psetu2(r, d0, d1);
#else
			if ((n = (u != 0)) && r)
				hebi_psetu(r, u);
#endif
		} else {
			/* 2x1 division by normalized reciprocal */
			d0 = d0 << shft;
			v = RECIPU_2x1(d0);
			d0 = PDIVREMRU_2x1(ql, al, an, shft, d0, v);

			/* store remainder */
			if ((n = (d0 != 0)) && r)
				hebi_psetu(r, d0);
		}

	}

	/* store length of remainder and normalize length of quotient */
	if (rn)
		*rn = n;

	if (UNLIKELY(!m))
		return 0;

	return hebi_pnorm(q, m);
}
