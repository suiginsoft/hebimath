/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

/*
 * functions for writing optional radix prefix for the different
 * alphabet encodings
 */

static inline size_t
write(char *str, size_t cur, size_t end, char c)
{
	if (LIKELY(cur < end)) {
		str[cur] = c;
		return cur + 1;
	}

	return cur;
}

static size_t
defaultwriteradix(
		char *restrict str,
		size_t *restrict cur,
		size_t end,
		unsigned int base )
{
	size_t rlen;

	if (base == 16) {
		*cur = write(str, *cur, end, '0');
		*cur = write(str, *cur, end, 'x');
		rlen = 2;
	} else if (base == 8) {
		*cur = write(str, *cur, end, '0');
		rlen = 1;
	} else if (base == 2) {
		*cur = write(str, *cur, end, '0');
		*cur = write(str, *cur, end, 'b');
		rlen = 2;
	} else {
		rlen = 0;
	}

	return rlen;
}

static size_t
rfc4648writeradix(
		char *restrict str,
		size_t *restrict cur,
		size_t end,
		unsigned int base )
{
	IGNORE(str, cur, end, base);
	return 0;
}

/* lookup tables for each alphabet for converting a digit to a character */
static const char base36upperlut[36] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V',
	'W', 'X', 'Y', 'Z'
};

static const char base36lowerlut[36] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r',
	's', 't', 'u', 'v',
	'w', 'x', 'y', 'z'
};

static const char base62lut[62] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V',
	'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p',
	'q', 'r', 's', 't',
	'u', 'v', 'w', 'x',
	'y', 'z'
};

static const char rfc4648base32lut[32] = {
	'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X',
	'Y', 'Z', '2', '3',
	'4', '5', '6', '7'
};

static const char rfc4648base64lut[64] = {
	'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b',
	'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r',
	's', 't', 'u', 'v',
	'w', 'x', 'y', 'z',
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', '+', '/'
};

/* parameters to describe how to encode a number for a specific alphabet */
struct alphabet_encoder {
	size_t (* writeradix)(char *, size_t *, size_t, unsigned int);
	const char *alphalut;
	unsigned int maxradix;
	char pad;
	char zero;
};

/* table of encoding parameters for each alphabet */
static const struct alphabet_encoder encoders[HEBI_STR_ALPHABET_COUNT] = {
	{ &defaultwriteradix, base36upperlut, 36, '\0', '0' },
	{ &defaultwriteradix, base36upperlut, 36, '\0', '0' },
	{ &defaultwriteradix, base36lowerlut, 36, '\0', '0' },
	{ &defaultwriteradix, base62lut, 62, '\0', '0' },
	{ &rfc4648writeradix, rfc4648base32lut, 32, '=', 'A' },
	{ &rfc4648writeradix, rfc4648base64lut, 64, '=', 'A' }
};

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
	const struct alphabet_encoder *encoder;
	const char *alphalut;   /* lookup tables for encoding */
	unsigned int alphabet;  /* alphabet encoder index */
	unsigned int bits;      /* leading zero bits of base */
	MLIMB d;                /* base normalized for division */
	MLIMB v;                /* reciprocal estimate of d */
	size_t wn;
	size_t rlen;
	size_t start;
	size_t end;
	size_t cur;
	int digit;

	ASSERT(n <= HEBI_PACKET_MAXLEN);
	ASSERT(2 <= base && base <= 64);

	/* determine alphabet encoder */
	alphabet = flags & HEBI_STR_ALPHABET_MASK;
	if (UNLIKELY(alphabet >= HEBI_STR_ALPHABET_COUNT))
		return SIZE_MAX;
	encoder = &encoders[alphabet];

	/* make sure radix is supported by encoding */
	if (UNLIKELY(base < 2 || encoder->maxradix < base))
		return SIZE_MAX;

	/* setup result length and indices */
	rlen = 0;
	cur = 0;
	end = len - (len > 0);

	/* write out optional radix prefix */
	if (flags & HEBI_STR_RADIX)
		rlen = (*encoder->writeradix)(str, &cur, end, base);

	/* special case for zero-length input packet sequence */
	if (UNLIKELY(!n)) {
		if (LIKELY(!rlen || base != 8)) {
			cur = write(str, cur, end, encoder->zero);
			rlen++;
		}
		if (LIKELY(len > 0))
			str[cur] = '\0';
		return rlen;
	}

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
	alphalut = encoder->alphalut;
	do {
		for (cur = start; cur < end && wn > 0; cur++) {
			digit = divrem(w, &wn, bits, d, v);
			str[cur] = alphalut[digit];
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
