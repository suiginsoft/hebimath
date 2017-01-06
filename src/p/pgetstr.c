/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

static
void
reverse(char *str, size_t first, size_t last)
{
	size_t i = first;
	size_t j = last;

	while (i < j) {
		SWAP(char, str[i], str[j]);
		i++;
		j--;
	}
}

static inline
size_t
write(char *str, size_t cur, size_t end, char c)
{
	if (LIKELY(cur < end)) {
		str[cur] = c;
		return cur + 1;
	}

	return cur;
}

HEBI_API
size_t
hebi_pgetstr(
		char *restrict str,
		size_t len,
		hebi_packet *restrict w,
		size_t n,
		int base )
{
	const unsigned int flags = (unsigned int)base;
	const unsigned int ubase = flags & HEBI_STR_BASEMASK;

	int digit;
	int letterbase;
	unsigned int bits;      /* leading zero bits of ubase */
	MLIMB d;                /* ubase normalized for division */
	MLIMB v;                /* reciprocal estimate of d */
	MLIMB *wl;
	size_t wln;
	size_t rlen;
	size_t start;
	size_t end;
	size_t cur;

	ASSERT(n <= HEBI_PACKET_MAXLEN);
	ASSERT(2 <= ubase && ubase <= 62);

	/* setup result length and indices */
	rlen = 0;
	cur = 0;
	end = len - (len > 0);

	/* write out optional radix prefix */
	if (flags & HEBI_STR_PREFIX) {
		if (ubase == 16) {
			cur = write(str, cur, end, '0');
			cur = write(str, cur, end, 'x');
			rlen += 2;
		} else if (ubase == 8) {
			cur = write(str, cur, end, '0');
			rlen++;
		} else if (ubase == 2) {
			cur = write(str, cur, end, '0');
			cur = write(str, cur, end, 'b');
			rlen += 2;
		}
	}

	/* special case for zero-length input packet sequence */
	if (UNLIKELY(!n)) {
		if (LIKELY(!rlen || ubase != 8)) {
			cur = write(str, cur, end, '0');
			rlen++;
		}
		if (LIKELY(len > 0))
			str[cur] = '\0';
		return rlen;
	}

	/* determine lowercase or uppercase */
	letterbase = 'a' - 10;
	if (flags & HEBI_STR_UPPER)
		letterbase = 'A' - 10;

	/* compute reciprocal and normalized divisor */
	d = (MLIMB)ubase;
	bits = MLIMB_CLZ(d);
	d = d << bits;
	v = RECIPU_2x1(d);

	/* setup input limb pointer/counter */
	wl = MLIMB_PTR(w);
	wln = n;

	/*
	 * if we have no more space in output string, just consume
	 * digits in order to compute final result length
	 */
	if (UNLIKELY(cur >= end)) {
		/* null-terminate output */
		if (len > 0)
			str[cur] = '\0';

		/* consume digits */
		while (wln > 0) {
			wln *= MLIMB_PER_PACKET;
			(void)PDIVREMRU_2x1(wl, wl, wln, bits, d, v);
			rlen++;
			wln = hebi_pnorm(w, wln / MLIMB_PER_PACKET);
		}

		return rlen;
	}

	/*
	 * otherwise, compute and write character digits to string. wrap
	 * around in ring-buffer fashion if we run out of room: when the
	 * digits are reversed, the truncated string will then have the
	 * correct sequence of truncated digits
	 */
	start = cur;
	do {
		for (cur = start; cur < end && wln > 0; cur++) {
			wln *= MLIMB_PER_PACKET;
			digit = (int)PDIVREMRU_2x1(wl, wl, wln, bits, d, v);
			if (digit < 10)
				digit += '0';
			else if (ubase <= 36)
				digit += letterbase;
			else if (digit < 36)
				digit += 'A' - 10;
			else
				digit += 'a' - 36;
			str[cur] = (char)digit;
			rlen++;
			wln = hebi_pnorm(w, wln / MLIMB_PER_PACKET);
		}
	} while (UNLIKELY(wln > 0));

	/*
	 * reverse the digits to arrive at a right-to-left ordered sequence
	 * and null-terminate the output string
	 */
	if (UNLIKELY(rlen >= len)) {
		reverse(str, start, cur - 1);
		start = cur;
		cur = len - 1;
	}

	str[cur] = '\0';
	reverse(str, start, cur - 1);
	return rlen;
}
