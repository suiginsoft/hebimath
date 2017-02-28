/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

static inline char
digtochar(int digit, int lettering, unsigned int base)
{
	int c;

	if (digit < 10)
		c = digit + '0';
	else if (base <= 36)
		c = digit + lettering;
	else if (digit < 36)
		c = digit + 'A' - 10;
	else
		c = digit + 'a' - 36;

	return (char)c;
}

static inline int
divrem(hebi_packet *w, size_t *n, unsigned int bits, MLIMB d, MLIMB v)
{
	MLIMB *wl;
	size_t wn;
	size_t wln;
	int rem;

	wl = MLIMB_PTR(w);
	wn = *n;
	wln = wn * MLIMB_PER_PACKET;

	rem = (int)PDIVREMRU_2X1(wl, wl, wln, bits, d, v);
	wn = hebi_pnorm(w, wn);

	*n = wn;
	return rem;
}

static inline void
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

static inline size_t
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
		unsigned int base,
		unsigned int flags )
{
	unsigned int bits;      /* leading zero bits of base */
	MLIMB d;                /* base normalized for division */
	MLIMB v;                /* reciprocal estimate of d */
	size_t wn;
	size_t rlen;
	size_t start;
	size_t end;
	size_t cur;
	int digit;
	int lettering;

	ASSERT(n <= HEBI_PACKET_MAXLEN);
	ASSERT(2 <= base && base <= 64);

	/* setup result length and indices */
	rlen = 0;
	cur = 0;
	end = len - (len > 0);

	/* write out optional radix prefix */
	if (flags & HEBI_STR_RADIX) {
		if (base == 16) {
			cur = write(str, cur, end, '0');
			cur = write(str, cur, end, 'x');
			rlen += 2;
		} else if (base == 8) {
			cur = write(str, cur, end, '0');
			rlen++;
		} else if (base == 2) {
			cur = write(str, cur, end, '0');
			cur = write(str, cur, end, 'b');
			rlen += 2;
		}
	}

	/* special case for zero-length input packet sequence */
	if (UNLIKELY(!n)) {
		if (LIKELY(!rlen || base != 8)) {
			cur = write(str, cur, end, '0');
			rlen++;
		}
		if (LIKELY(len > 0))
			str[cur] = '\0';
		return rlen;
	}

	/* determine lowercase or uppercase letters */
	lettering = 'a' - 10;
	if ((flags & HEBI_STR_ALPHABET_MASK) == HEBI_STR_BASE36_UPPER)
		lettering = 'A' - 10;

	/* compute reciprocal and normalized divisor */
	d = (MLIMB)base;
	bits = MLIMB_CLZ(d);
	d = d << bits;
	v = RECIPU_2X1(d);
	wn = n;

	/*
	 * if we have no more space in output string, just consume
	 * digits in order to compute final result length
	 */
	if (UNLIKELY(cur >= end)) {
		if (len > 0)
			str[cur] = '\0';
		while (wn > 0) {
			(void)divrem(w, &wn, bits, d, v);
			rlen++;
		}
		return rlen;
	}

	/*
	 * otherwise, compute and write character digits to string. wrap
	 * around in ring-buffer fashion if we run out of room: when the
	 * digits are reversed, the truncated string will then contain
	 * the correct sequence
	 */
	start = cur;
	do {
		for (cur = start; cur < end && wn > 0; cur++) {
			digit = divrem(w, &wn, bits, d, v);
			str[cur] = digtochar(digit, lettering, base);
			rlen++;
		}
	} while (UNLIKELY(wn > 0));

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
