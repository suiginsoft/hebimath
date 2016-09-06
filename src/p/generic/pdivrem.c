/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

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
	int bits;
	size_t limbs, m, n;
	MLIMB v;

	/* determine number of bits needed to normalize divisor */
	n = hebi_pclz(b, bn);
	bits = (int)(n % MLIMB_BIT);
	limbs = n / MLIMB_BIT;
	n = bn * MLIMB_PER_PACKET - limbs;

	if (UNLIKELY(!n)) {
		if (rn)
			*rn = 0;
		return 0;
	}

	if (n > 2) {
		/* division with large multi-word divisor */
		hebi_packet *restrict u, *restrict d;
		MLIMB *restrict ul;
		const MLIMB *restrict dl;

		/* normalize divisor and dividend into working memory */
		u = w;
		m = hebi_pshl(u, a, bits, an) * MLIMB_PER_PACKET;

		d = w + an + 1;
		hebi_pshl(d, b, bits, bn);

		ul = MLIMB_PTR(u);
		dl = MLIMB_PTR(d);

		/* perform normalized multi-word division */
		v = RECIPU_3x2(dl[n - 1], dl[n - 2]);
		m = PDIVREMR(q, ul, dl, v, m, n);
		n = (n + MLIMB_PER_PACKET - 1) / MLIMB_PER_PACKET;

		/* denormalize remainder if caller provided space for it */
		if (r || rn) {
			bits = (MLIMB_BIT - bits) & (MLIMB_BIT - 1);
			n = hebi_pshr(r ? r : u, u, bits, n);
		}
	} else {
		/* division with small divisor */
		MLIMB *restrict ql;
		const MLIMB *al;
		const MLIMB *bl;
		MLIMB d1, d0;

		ql = MLIMB_PTR(q);
		al = MLIMB_PTR(a);
		bl = MLIMB_PTR(b);

		m = an;
		an = an * MLIMB_PER_PACKET;
		d0 = bl[0];

		if (n == 2) {
			/* 3x2 division by normalized reciprocal */
			DLIMB u;
			d1 = bl[1];
			if (bits) {
				d1 = (d1 << bits) | (d0 >> (MLIMB_BIT - bits));
				d0 = d0 << bits;
			}
			v = RECIPU_3x2(d1, d0);
			u = PDIVREMRU_3x2(ql, al, an, bits, d1, d0, v);
#ifdef USE_LIMB64_MULDIV
			d0 = (MLIMB)(u & MLIMB_MAX);
			d1 = (MLIMB)(u >> MLIMB_BIT);
			n = d0 || d1 ? 1 : 0;
#else
			d0 = u;
			d1 = 0;
			n = d0 ? 1 : 0;
#endif
		} else {
			/* 2x1 division by normalized reciprocal */
			d0 = d0 << bits;
			v = RECIPU_2x1(d0);
			d0 = PDIVREMRU_2x1(ql, al, an, bits, d0, v);
			d1 = 0;
			n = d0 ? 1 : 0;
		}

		if (r && n)
			hebi_psetu2(r, d0, d1);
	}

	/* store length of remainder and determine length of quotient */
	if (rn)
		*rn = n;

	return hebi_pnorm(q, m);
}
