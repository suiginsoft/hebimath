/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"
#include <ctype.h>

/* hebi_psetstr requires the following constraints from the language & libraries */
STATIC_ASSERT(UCHAR_MAX <= UINT8_MAX, "uchar must be 8-bits or less");
STATIC_ASSERT(UINT_MAX <= MLIMB_MAX, "MLIMB should be at least as large as uint");

/*
 * lookup tables for converting from number of digits in a given base
 * to number of packets, represented as 64-bit or 32-bit fixed-point
 * approximation
 *
 * both tables are generated with the program fragment below
 */

#if 0

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	long bits;
	long double scale, x;
	int i, j, r;
	if (argc < 2 || !argv[1]) {
		fputs("expected number of bits for size_t\n", stderr);
		return -1;
	}
	errno = 0;
	bits = strtol(argv[1], NULL, 10);
	if ((bits != 32 && bits != 64) || errno == ERANGE) {
		fputs("invalid number of bits for size_t\n", stderr);
		return -1;
	}
	scale = pow(2.0l, (long double)bits);
	printf("static const uint%ld_t digittopacketlut%ld[64] = {\n", bits, bits);
	for (i = 0, j = 0; i < 64; i += j) {
		putchar('\t');
		for (j = 0; j < 2 && (i + j) < 64; j++) {
			r = i + j + 1;
			if (2 <= r && r <= 64) {
				x = ceill((logl(r) / (128.0l * logl(2.0l))) * scale);
			} else {
				x = 0.0l;
			}
			if (bits == 64)
				printf("UINT64_C(0x%016" PRIX64 ")", (uint64_t)x);
			else
				printf("UINT32_C(0x%08" PRIX32 ")", (uint32_t)x);
			if (r != 64)
				fputs(j == 1 ? "," : ", ", stdout);
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

#if SIZE_MAX == UINT64_MAX

static const uint64_t digittopacketlut64[64] = {
	UINT64_C(0x0000000000000000), UINT64_C(0x0200000000000000),
	UINT64_C(0x032B803473F7AD10), UINT64_C(0x0400000000000000),
	UINT64_C(0x04A4D3C25E68DC58), UINT64_C(0x052B803473F7AD10),
	UINT64_C(0x059D5D9FD5010B37), UINT64_C(0x0600000000000000),
	UINT64_C(0x06570068E7EF5A1F), UINT64_C(0x06A4D3C25E68DC58),
	UINT64_C(0x06EB3A9F01975078), UINT64_C(0x072B803473F7AD10),
	UINT64_C(0x0766A008E4788CBD), UINT64_C(0x079D5D9FD5010B37),
	UINT64_C(0x07D053F6D2608968), UINT64_C(0x0800000000000000),
	UINT64_C(0x082CC7EDF592262D), UINT64_C(0x08570068E7EF5A1F),
	UINT64_C(0x087EF05AE409A029), UINT64_C(0x08A4D3C25E68DC58),
	UINT64_C(0x08C8DDD448F8B846), UINT64_C(0x08EB3A9F01975078),
	UINT64_C(0x090C10500D63AA66), UINT64_C(0x092B803473F7AD10),
	UINT64_C(0x0949A784BCD1B8B0), UINT64_C(0x0966A008E4788CBD),
	UINT64_C(0x0982809D5BE7072E), UINT64_C(0x099D5D9FD5010B37),
	UINT64_C(0x09B74948F5532DA5), UINT64_C(0x09D053F6D2608968),
	UINT64_C(0x09E88C6B3626A72B), UINT64_C(0x0A00000000000000),
	UINT64_C(0x0A16BAD3758EFD88), UINT64_C(0x0A2CC7EDF592262D),
	UINT64_C(0x0A4231623369E78F), UINT64_C(0x0A570068E7EF5A1F),
	UINT64_C(0x0A6B3D78B6D3B250), UINT64_C(0x0A7EF05AE409A029),
	UINT64_C(0x0A92203D587039CD), UINT64_C(0x0AA4D3C25E68DC58),
	UINT64_C(0x0AB7110E6CE866F3), UINT64_C(0x0AC8DDD448F8B846),
	UINT64_C(0x0ADA3F5FB9C41506), UINT64_C(0x0AEB3A9F01975078),
	UINT64_C(0x0AFBD42B46583677), UINT64_C(0x0B0C10500D63AA66),
	UINT64_C(0x0B1BF311E95D00DF), UINT64_C(0x0B2B803473F7AD10),
	UINT64_C(0x0B3ABB3FAA02166D), UINT64_C(0x0B49A784BCD1B8B0),
	UINT64_C(0x0B5848226989D33D), UINT64_C(0x0B66A008E4788CBD),
	UINT64_C(0x0B74B1FD64E0753D), UINT64_C(0x0B82809D5BE7072E),
	UINT64_C(0x0B900E6160002CD0), UINT64_C(0x0B9D5D9FD5010B37),
	UINT64_C(0x0BAA708F58014D38), UINT64_C(0x0BB74948F5532DA5),
	UINT64_C(0x0BC3E9CA2E1A0554), UINT64_C(0x0BD053F6D2608968),
	UINT64_C(0x0BDC899AB3FF56C6), UINT64_C(0x0BE88C6B3626A72B),
	UINT64_C(0x0BF45E08BCF06555), UINT64_C(0x0C00000000000000)
};

#elif SIZE_MAX == UINT32_MAX

static const uint32_t digittopacketlut32[64] = {
	UINT32_C(0x00000000), UINT32_C(0x02000000),
	UINT32_C(0x032B8035), UINT32_C(0x04000000),
	UINT32_C(0x04A4D3C3), UINT32_C(0x052B8035),
	UINT32_C(0x059D5DA0), UINT32_C(0x06000000),
	UINT32_C(0x06570069), UINT32_C(0x06A4D3C3),
	UINT32_C(0x06EB3AA0), UINT32_C(0x072B8035),
	UINT32_C(0x0766A009), UINT32_C(0x079D5DA0),
	UINT32_C(0x07D053F7), UINT32_C(0x08000000),
	UINT32_C(0x082CC7EE), UINT32_C(0x08570069),
	UINT32_C(0x087EF05B), UINT32_C(0x08A4D3C3),
	UINT32_C(0x08C8DDD5), UINT32_C(0x08EB3AA0),
	UINT32_C(0x090C1051), UINT32_C(0x092B8035),
	UINT32_C(0x0949A785), UINT32_C(0x0966A009),
	UINT32_C(0x0982809E), UINT32_C(0x099D5DA0),
	UINT32_C(0x09B74949), UINT32_C(0x09D053F7),
	UINT32_C(0x09E88C6C), UINT32_C(0x0A000000),
	UINT32_C(0x0A16BAD4), UINT32_C(0x0A2CC7EE),
	UINT32_C(0x0A423163), UINT32_C(0x0A570069),
	UINT32_C(0x0A6B3D79), UINT32_C(0x0A7EF05B),
	UINT32_C(0x0A92203E), UINT32_C(0x0AA4D3C3),
	UINT32_C(0x0AB7110F), UINT32_C(0x0AC8DDD5),
	UINT32_C(0x0ADA3F60), UINT32_C(0x0AEB3AA0),
	UINT32_C(0x0AFBD42C), UINT32_C(0x0B0C1051),
	UINT32_C(0x0B1BF312), UINT32_C(0x0B2B8035),
	UINT32_C(0x0B3ABB40), UINT32_C(0x0B49A785),
	UINT32_C(0x0B584823), UINT32_C(0x0B66A009),
	UINT32_C(0x0B74B1FE), UINT32_C(0x0B82809E),
	UINT32_C(0x0B900E62), UINT32_C(0x0B9D5DA0),
	UINT32_C(0x0BAA7090), UINT32_C(0x0BB74949),
	UINT32_C(0x0BC3E9CB), UINT32_C(0x0BD053F7),
	UINT32_C(0x0BDC899B), UINT32_C(0x0BE88C6C),
	UINT32_C(0x0BF45E09), UINT32_C(0x0C000000)
};

#else

#error "no digit-to-packet lookup table matching size of size_t!"

#endif

/*
 * lookup tables to determine the maximum number of digits of a particular
 * base that can safely be stored in 64-bit limb without causing overflow,
 * as well as lookup tables with values containing the base raised to the
 * power of the max number of digits
 *
 * both tables are generated with the program below
 */

#if 0

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static long double digits[64];
static long double scale[64];

int
main(int argc, char **argv)
{
	const long double bits = 64.0l;
	long double logb2;
	long double p;
	int b, j;
	for (b = 2; b <= 64; b++) {
		logb2 = logl(2.0l) / logl((long double)b);
		digits[b-1] = floorl(bits * logb2);
		if ((b & (b-1)) == 0) {
			digits[b-1] /= logb2;
			scale[b-1] = 0.0l;
		} else {
			scale[b-1] = floorl(powl((long double)b, digits[b-1]));
		}
	}
	puts("static const uint8_t maxdigitslut[64] = {");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 8 && (b+j) < 64; j++) {
			printf("0x%02" PRIX8, (uint8_t)digits[b+j]);
			if ((b+j) != 63)
				printf(j == 7 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};\n");
	puts("static const uint64_t maxscalelut[64] = {");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 2 && (b+j) < 64; j++) {
			printf("UINT64_C(0x%016" PRIX64 ")", (uint64_t)scale[b+j]);
			if ((b+j) != 63)
				printf(j == 1 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

static const uint8_t maxdigitslut[64] = {
	0x00, 0x40, 0x28, 0x40, 0x1B, 0x18, 0x16, 0x3F,
	0x14, 0x13, 0x12, 0x11, 0x11, 0x10, 0x10, 0x40,
	0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0D,
	0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0C, 0x3C,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x3C
};

static const uint64_t maxscalelut[64] = {
	UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
	UINT64_C(0xA8B8B452291FE821), UINT64_C(0x0000000000000000),
	UINT64_C(0x6765C793FA10079D), UINT64_C(0x41C21CB8E1000000),
	UINT64_C(0x3642798750226111), UINT64_C(0x0000000000000000),
	UINT64_C(0xA8B8B452291FE821), UINT64_C(0x8AC7230489E80000),
	UINT64_C(0x4D28CB56C33FA539), UINT64_C(0x1ECA170C00000000),
	UINT64_C(0x780C7372621BD74D), UINT64_C(0x1E39A5057D810000),
	UINT64_C(0x5B27AC993DF97701), UINT64_C(0x0000000000000000),
	UINT64_C(0x27B95E997E21D9F1), UINT64_C(0x5DA0E1E53C5C8000),
	UINT64_C(0xD2AE3299C1C4AEDB), UINT64_C(0x16BCC41E90000000),
	UINT64_C(0x2D04B7FDD9C0EF49), UINT64_C(0x5658597BCAA24000),
	UINT64_C(0xA0E2073737609371), UINT64_C(0x0C29E98000000000),
	UINT64_C(0x14ADF4B7320334B9), UINT64_C(0x226ED36478BFA000),
	UINT64_C(0x383D9170B85FF80B), UINT64_C(0x5A3C23E39C000000),
	UINT64_C(0x8E65137388122BCD), UINT64_C(0xDD41BB36D259E000),
	UINT64_C(0x0AEE5720EE830681), UINT64_C(0x0000000000000000),
	UINT64_C(0x172588AD4F5F0981), UINT64_C(0x211E44F7D02C1000),
	UINT64_C(0x2EE56725F06E5C71), UINT64_C(0x41C21CB8E1000000),
	UINT64_C(0x5B5B57F8A98A5DD1), UINT64_C(0x7DCFF8986EA31000),
	UINT64_C(0xABD4211662A6B2A1), UINT64_C(0xE8D4A51000000000),
	UINT64_C(0x07A32956AD081B79), UINT64_C(0x09F49AAFF0E86800),
	UINT64_C(0x0CE583BB812D37B3), UINT64_C(0x109B79A654C00000),
	UINT64_C(0x1543BEFF214C8B95), UINT64_C(0x1B149A79459A3800),
	UINT64_C(0x224EDFB5434A830F), UINT64_C(0x2B3FB00000000000),
	UINT64_C(0x3642798750226111), UINT64_C(0x43C33C1937564800),
	UINT64_C(0x54411B2441C3CD8B), UINT64_C(0x6851455ACD400000),
	UINT64_C(0x80A23B117C8FEB6D), UINT64_C(0x9DFF7D32D5DC1800),
	UINT64_C(0xC155AF6FAEFFE6A7), UINT64_C(0xEBB7392E00000000),
	UINT64_C(0x050633659656D971), UINT64_C(0x05FA8624C7FBA400),
	UINT64_C(0x0717D9FAA73C5679), UINT64_C(0x086430AAC6100000),
	UINT64_C(0x09E64D9944B57F29), UINT64_C(0x0BA5CA5392CB0400),
	UINT64_C(0x0DAB2CE1D022CD81), UINT64_C(0x0000000000000000)
};

/*
 * functions for reading optional radix prefix and determining
 * the default radix if no prefix exists for different alphabets
 */

static inline int
peek(const char *str, size_t cur, size_t len, char c)
{
	if (LIKELY(cur < len))
		return str[cur] == c;
	return 0;
}

static inline int
peekcase(const char *str, size_t cur, size_t len, char c0, char c1)
{
	if (LIKELY(cur < len))
		return str[cur] == c0 || str[cur] == c1;
	return 0;
}

static inline int
peekradix(const char *str, size_t cur, size_t len, char c0, char c1)
{
	return peek(str, cur, len, '0') && peekcase(str, cur + 1, len, c0, c1);
}

static unsigned int
defaultreadradix(
		const char *restrict str,
		size_t *restrict cur,
		size_t len,
		unsigned int base )
{
	unsigned int radix = base;

	if (!radix) {
		radix = 10;
		if (peek(str, *cur, len, '0')) {
			radix -= 2;
			(*cur)++;
			if (peekcase(str, *cur, len, 'x', 'X')) {
				radix *= 2;
				(*cur)++;
			} else if (peekcase(str, *cur, len, 'b', 'B')) {
				radix /= 4;
				(*cur)++;
			}
		}
	} else if ((radix == 16 && peekradix(str, *cur, len, 'x', 'X')) ||
			(radix == 2 && peekradix(str, *cur, len, 'b', 'B'))) {
		*cur += 2;
	}

	return radix;
}

static unsigned int
rfc4648base32readradix(
		const char *restrict str,
		size_t *restrict cur,
		size_t len,
		unsigned int base )
{
	IGNORE(str, cur, len);
	return base ? base : 32;
}

static unsigned int
rfc4648base64readradix(
		const char *restrict str,
		size_t *restrict cur,
		size_t len,
		unsigned int base )
{
	IGNORE(str, cur, len);
	return base ? base : 64;
}

/*
 * character to digit conversion tables for different encoding alphabets
 *
 * here we rely on unsigned integer wrapping of the value fetched from
 * the lookup table by subtracting one, resulting in invalid characters
 * yielding a return value greater than the allowed digit range
 */

static const unsigned char base36digitlut[256] = {
	['0'] = 1,  ['1'] = 2,  ['2'] = 3,  ['3'] = 4,
	['4'] = 5,  ['5'] = 6,  ['6'] = 7,  ['7'] = 8,
	['8'] = 9,  ['9'] = 10, ['A'] = 11, ['B'] = 12,
	['C'] = 13, ['D'] = 14, ['E'] = 15, ['F'] = 16,
	['G'] = 17, ['H'] = 18, ['I'] = 19, ['J'] = 20,
	['K'] = 21, ['L'] = 22, ['M'] = 23, ['N'] = 24,
	['O'] = 25, ['P'] = 26, ['Q'] = 27, ['R'] = 28,
	['S'] = 29, ['T'] = 30, ['U'] = 31, ['V'] = 32,
	['W'] = 33, ['X'] = 34, ['Y'] = 35, ['Z'] = 36,
	['a'] = 11, ['b'] = 12, ['c'] = 13, ['d'] = 14,
	['e'] = 15, ['f'] = 16, ['g'] = 17, ['h'] = 18,
	['i'] = 19, ['j'] = 20, ['k'] = 21, ['l'] = 22,
	['m'] = 23, ['n'] = 24, ['o'] = 25, ['p'] = 26,
	['q'] = 27, ['r'] = 28, ['s'] = 29, ['t'] = 30,
	['u'] = 31, ['v'] = 32, ['w'] = 33, ['x'] = 34,
	['y'] = 35, ['z'] = 36
};

static const unsigned char base36upperdigitlut[256] = {
	['0'] = 1,  ['1'] = 2,  ['2'] = 3,  ['3'] = 4,
	['4'] = 5,  ['5'] = 6,  ['6'] = 7,  ['7'] = 8,
	['8'] = 9,  ['9'] = 10, ['A'] = 11, ['B'] = 12,
	['C'] = 13, ['D'] = 14, ['E'] = 15, ['F'] = 16,
	['G'] = 17, ['H'] = 18, ['I'] = 19, ['J'] = 20,
	['K'] = 21, ['L'] = 22, ['M'] = 23, ['N'] = 24,
	['O'] = 25, ['P'] = 26, ['Q'] = 27, ['R'] = 28,
	['S'] = 29, ['T'] = 30, ['U'] = 31, ['V'] = 32,
	['W'] = 33, ['X'] = 34, ['Y'] = 35, ['Z'] = 36
};

static const unsigned char base36lowerdigitlut[256] = {
	['0'] = 1,  ['1'] = 2,  ['2'] = 3,  ['3'] = 4,
	['4'] = 5,  ['5'] = 6,  ['6'] = 7,  ['7'] = 8,
	['8'] = 9,  ['9'] = 10, ['a'] = 11, ['b'] = 12,
	['c'] = 13, ['d'] = 14, ['e'] = 15, ['f'] = 16,
	['g'] = 17, ['h'] = 18, ['i'] = 19, ['j'] = 20,
	['k'] = 21, ['l'] = 22, ['m'] = 23, ['n'] = 24,
	['o'] = 25, ['p'] = 26, ['q'] = 27, ['r'] = 28,
	['s'] = 29, ['t'] = 30, ['u'] = 31, ['v'] = 32,
	['w'] = 33, ['x'] = 34, ['y'] = 35, ['z'] = 36
};

static const unsigned char base62digitlut[256] = {
	['0'] = 1,  ['1'] = 2,  ['2'] = 3,  ['3'] = 4,
	['4'] = 5,  ['5'] = 6,  ['6'] = 7,  ['7'] = 8,
	['8'] = 9,  ['9'] = 10, ['A'] = 11, ['B'] = 12,
	['C'] = 13, ['D'] = 14, ['E'] = 15, ['F'] = 16,
	['G'] = 17, ['H'] = 18, ['I'] = 19, ['J'] = 20,
	['K'] = 21, ['L'] = 22, ['M'] = 23, ['N'] = 24,
	['O'] = 25, ['P'] = 26, ['Q'] = 27, ['R'] = 28,
	['S'] = 29, ['T'] = 30, ['U'] = 31, ['V'] = 32,
	['W'] = 33, ['X'] = 34, ['Y'] = 35, ['Z'] = 36,
	['a'] = 37, ['b'] = 38, ['c'] = 39, ['d'] = 40,
	['e'] = 41, ['f'] = 42, ['g'] = 43, ['h'] = 44,
	['i'] = 45, ['j'] = 46, ['k'] = 47, ['l'] = 48,
	['m'] = 49, ['n'] = 50, ['o'] = 51, ['p'] = 52,
	['q'] = 53, ['r'] = 54, ['s'] = 55, ['t'] = 56,
	['u'] = 57, ['v'] = 58, ['w'] = 59, ['x'] = 60,
	['y'] = 61, ['z'] = 62
};

static const unsigned char rfc4648base32digitlut[256] = {
	['A'] = 1,  ['B'] = 2,  ['C'] = 3,  ['D'] = 4,
	['E'] = 5,  ['F'] = 6,  ['G'] = 7,  ['H'] = 8,
	['I'] = 9,  ['J'] = 10, ['K'] = 11, ['L'] = 12,
	['M'] = 13, ['N'] = 14, ['O'] = 15, ['P'] = 16,
	['Q'] = 17, ['R'] = 18, ['S'] = 19, ['T'] = 20,
	['U'] = 21, ['V'] = 22, ['W'] = 23, ['X'] = 24,
	['Y'] = 25, ['Z'] = 26, ['2'] = 27, ['3'] = 28,
	['4'] = 29, ['5'] = 30, ['6'] = 31, ['7'] = 32
};

static const unsigned char rfc4648base64digitlut[256] = {
	['A'] = 1,  ['B'] = 2,  ['C'] = 3,  ['D'] = 4,
	['E'] = 5,  ['F'] = 6,  ['G'] = 7,  ['H'] = 8,
	['I'] = 9,  ['J'] = 10, ['K'] = 11, ['L'] = 12,
	['M'] = 13, ['N'] = 14, ['O'] = 15, ['P'] = 16,
	['Q'] = 17, ['R'] = 18, ['S'] = 19, ['T'] = 20,
	['U'] = 21, ['V'] = 22, ['W'] = 23, ['X'] = 24,
	['Y'] = 25, ['Z'] = 26, ['a'] = 27, ['b'] = 28,
	['c'] = 29, ['d'] = 30, ['e'] = 31, ['f'] = 32,
	['g'] = 33, ['h'] = 34, ['i'] = 35, ['j'] = 36,
	['k'] = 37, ['l'] = 38, ['m'] = 39, ['n'] = 40,
	['o'] = 41, ['p'] = 42, ['q'] = 43, ['r'] = 44,
	['s'] = 45, ['t'] = 46, ['u'] = 47, ['v'] = 48,
	['w'] = 49, ['x'] = 50, ['y'] = 51, ['z'] = 52,
	['0'] = 53, ['1'] = 54, ['2'] = 55, ['3'] = 56,
	['4'] = 57, ['5'] = 58, ['6'] = 59, ['7'] = 60,
	['8'] = 61, ['9'] = 62, ['+'] = 63, ['/'] = 64
};

/* parameters that describe how to decode a number in a specific alphabet */
struct alphabet_decoder {
	unsigned int (* readradix)(const char *, size_t *, size_t, unsigned int);
	const unsigned char* digitlut;
	unsigned int maxradix;
	char minus;
	char plus;
	char pad;
	char zero;
};

/* table of decoding parameters for each alphabet */
static const struct alphabet_decoder decoders[HEBI_STR_ALPHABET_COUNT] = {
	{ &defaultreadradix, base36digitlut, 36, '-', '+', '\0', '0' },
	{ &defaultreadradix, base36upperdigitlut, 36, '-', '+', '\0', '0' },
	{ &defaultreadradix, base36lowerdigitlut, 36, '-', '+', '\0', '0' },
	{ &defaultreadradix, base62digitlut, 62, '-', '+', '\0', '0' },
	{ &rfc4648base32readradix, rfc4648base32digitlut, 32, '-', '+', '=', 'A' },
	{ &rfc4648base64readradix, rfc4648base64digitlut, 64, '-', '\0', '=', 'A' }
};

static inline size_t
trimleft(const char *restrict str, size_t start, size_t end, char c)
{
	size_t i = start;

	while (i < end && str[i] == c)
		i++;

	return i;
}

static inline size_t
trimright(const char *restrict str, size_t start, size_t end, char c)
{
	size_t i = end;

	while (start < i && str[i - 1] == c)
		i--;

	return i;
}

static inline void
trimwhitespace(const char *restrict str, size_t *restrict start, size_t *restrict end)
{
	size_t first = *start;
	size_t last = *end;

	while (first < last && isspace(str[first]))
		first++;

	while (first < last && isspace(str[last - 1]))
		last--;

	*start = first;
	*end = last;
}

static inline size_t
readsign(
		const struct alphabet_decoder *restrict decoder,
		struct hebi_psetstrstate *restrict state,
		const char *restrict str,
		size_t start,
		size_t end )
{
	size_t i = start;

	if (i < end) {
		if (decoder->minus != '\0' && str[i] == decoder->minus) {
			state->hm_sign = -1;
			i++;
		} else if (decoder->plus != '\0' && str[i] == decoder->plus) {
			state->hm_sign = 1;
			i++;
		}
	}

	return i;
}

static inline unsigned int
chartodigit(const unsigned char *digitlut, char c)
{
	return (unsigned int)digitlut[(unsigned char)c] - 1;
}

static size_t
error(struct hebi_psetstrstate *state, size_t cur, int code)
{
	state->hm_cur = cur;
	state->hm_errcode = code;
	return SIZE_MAX;
}

static size_t
estimatespace(size_t cur, size_t end, unsigned int radix)
{
	size_t space;

#if SIZE_MAX == UINT64_MAX
	uint64_t ratio;
	uint64_t spacehi;
#else
	uint32_t ratio;
#endif

#if SIZE_MAX == UINT64_MAX
	ratio = digittopacketlut64[radix - 1];
	(void)hebi_mulu128__(&spacehi, ratio, (uint64_t)(end - cur));
	space = (size_t)spacehi;
#else
	ratio = digittopacketlut32[radix - 1];
	space = (size_t)(((uint64_t)ratio * (uint32_t)(end - cur)) >> 32);
#endif

	return space;
}

static size_t
outofspace(struct hebi_psetstrstate *state, size_t cur, size_t sizethusfar)
{
	size_t space;

	state->hm_cur = cur;
	state->hm_errcode = HEBI_EBADLENGTH;

	space = estimatespace(cur, state->hm_end, state->hm_radix);
	if (UNLIKELY(space + sizethusfar < sizethusfar))
		return SIZE_MAX;

	space += sizethusfar;
	if (UNLIKELY(space > HEBI_PACKET_MAXLEN))
		return SIZE_MAX;

	return space;
}

#define ENSURE_SPACE_AVAILABLE(SIZE) \
	MULTILINEBEGIN \
	if (UNLIKELY((SIZE) > n)) \
		return outofspace(state, cur, (SIZE)); \
	MULTILINEEND

#define PUSH_LIMB(VAL) \
	MULTILINEBEGIN \
	if (VAL) { \
		size++; \
		ENSURE_SPACE_AVAILABLE(size); \
		hebi_psetu(r + size - 1, (VAL)); \
	} \
	MULTILINEEND

static size_t
readmultiply(
		hebi_packet *restrict r,
		size_t n,
		struct hebi_psetstrstate *restrict state,
		const struct alphabet_decoder *restrict decoder,
		unsigned int radix )
{
	const char *str;
	size_t cur;
	size_t end;
	const unsigned char *digitlut;
	uint64_t limb;
	uint64_t overflow;
	uint64_t scale;
	uint64_t maxscale;
	unsigned int digit;
	unsigned int numdigits;
	unsigned int maxdigits;
	size_t size;

	str = state->hm_str;
	cur = state->hm_cur;
	end = state->hm_end;
	digitlut = decoder->digitlut;
	maxdigits = maxdigitslut[radix - 1];
	maxscale = maxscalelut[radix - 1];
	size = 0;

	while (cur < end) {
		limb = 0;
		numdigits = 0;
		for ( ; cur < end && numdigits < maxdigits; cur++) {
			digit = chartodigit(digitlut, str[cur]);
			if (UNLIKELY(digit >= radix))
				return error(state, cur, HEBI_EBADSYNTAX);
			limb *= radix;
			limb += digit;
			numdigits++;
		}
		if (size > 0) {
			scale = maxscale;
			if (numdigits != maxdigits)
				scale = hebi_powu64__(radix, numdigits);
			overflow = hebi_pmulu(r, r, scale, size);
			PUSH_LIMB(overflow);
			limb = hebi_paddu(r, r, limb, size);
		}
		PUSH_LIMB(limb);
	}

	return size;
}

static size_t
readshift(
		hebi_packet *restrict r,
		size_t n,
		struct hebi_psetstrstate *restrict state,
		const struct alphabet_decoder *restrict decoder,
		unsigned int radix )
{
	const char *str;
	size_t cur;
	size_t end;
	const unsigned char *digitlut;
	uint64_t limb;
	unsigned int digit;
	unsigned int numdigits;
	unsigned int maxdigits;
	unsigned int radixbits;
	size_t size;

	str = state->hm_str;
	cur = state->hm_cur;
	end = state->hm_end;
	digitlut = decoder->digitlut;
	maxdigits = maxdigitslut[radix - 1];
	radixbits = (unsigned int)hebi_floorlog2sz__(radix);
	size = 0;

	while (cur < end) {
		limb = 0;
		numdigits = 0;
		for ( ; cur < end && numdigits < maxdigits; cur++) {
			digit = chartodigit(digitlut, str[cur]);
			if (UNLIKELY(digit >= radix))
				return error(state, cur, HEBI_EBADSYNTAX);
			limb <<= radixbits;
			limb |= digit;
			numdigits += radixbits;
		}
		if (size > 0) {
			ENSURE_SPACE_AVAILABLE(size + 1);
			size = hebi_pshl(r, r, numdigits, size);
			r->hp_limbs64[0] |= limb;
		} else if (limb) {
			ASSERT(!size && n > 0);
			hebi_psetu(r, limb);
			size++;
		}
	}

	return size;
}

HEBI_API
size_t
hebi_psetstr(
		hebi_packet *restrict r,
		size_t n,
		struct hebi_psetstrstate *restrict state )
{
	const struct alphabet_decoder *decoder;
	unsigned int radix;

	/* validate arguments */
	ASSERT(0 < n && n <= HEBI_PACKET_MAXLEN);

	ASSERT(state->hm_end <= state->hm_len);
	ASSERT(state->hm_start <= state->hm_end);
	ASSERT(state->hm_cur < state->hm_end);
	ASSERT(state->hm_errcode == HEBI_ENONE);

	ASSERT(state->hm_alphabet < HEBI_STR_ALPHABET_COUNT);
	decoder = &decoders[state->hm_alphabet];

	radix = state->hm_radix;
	ASSERT(2 <= radix && radix <= decoder->maxradix);

	/*
	 * check if power of two base and use bit-shifting to accumulate
	 * the result if this is the case, otherwise use multiplication
	 */
	if (radix & (radix - 1))
		return readmultiply(r, n, state, decoder, radix);
	else
		return readshift(r, n, state, decoder, radix);
}

HEBI_API
size_t
hebi_psetstrprepare(
		struct hebi_psetstrstate *restrict state,
		const char *restrict str,
		size_t len,
		unsigned int base,
		unsigned int flags )
{
	const struct alphabet_decoder *decoder;
	unsigned int radix;
	size_t cur;
	size_t end;
	size_t space;

	/* setup initial state */
	state->hm_str = str;
	state->hm_len = len;
	state->hm_start = 0;
	state->hm_end = len;
	state->hm_radix = 0;
	state->hm_sign = 1;

	cur = 0;
	end = len;

	/* determine alphabet index and make sure it's valid */
	state->hm_alphabet = flags & HEBI_STR_ALPHABET_MASK;
	if (UNLIKELY(state->hm_alphabet >= HEBI_STR_ALPHABET_COUNT))
		return error(state, cur, HEBI_EBADVALUE);
	decoder = &decoders[state->hm_alphabet];

	/* trim whitespace & padding, adjust start & end positions */
	if (flags & (unsigned int)(HEBI_STR_TRIM | HEBI_STR_PAD)) {
		if (flags & HEBI_STR_TRIM)
			trimwhitespace(str, &cur, &end);
		if ((flags & HEBI_STR_PAD) && decoder->pad != '\0')
			end = trimright(str, cur, end, decoder->pad);
		state->hm_start = cur;
		state->hm_end = len;
	}

	/* read sign character */
	if (flags & HEBI_STR_SIGN)
		cur = readsign(decoder, state, str, cur, end);

	/* determine radix, reading optional radix prefix if present */
	radix = base;
	if (!radix || (flags & HEBI_STR_RADIX))
		radix = (*decoder->readradix)(str, &cur, end, radix);
	state->hm_radix = radix;
	if (UNLIKELY(radix < 2 || decoder->maxradix < radix))
		return error(state, cur, HEBI_EBADVALUE);

	/* check for empty digit sequence */
	if (UNLIKELY(cur >= end))
		return error(state, cur, HEBI_EBADSYNTAX);

	/* consume leading zero characters and estimate remaining space */
	cur = trimleft(str, cur, end, decoder->zero);
	if (UNLIKELY(cur >= end)) {
		state->hm_cur = end;
		state->hm_sign = 0;
		state->hm_errcode = HEBI_ENONE;
		return 0;
	}

	/* estimate space needed for packet sequence */
	space = estimatespace(cur, end, radix);
	if (UNLIKELY(space >= HEBI_PACKET_MAXLEN))
		return error(state, cur, HEBI_EBADLENGTH);

	state->hm_cur = cur;
	state->hm_errcode = HEBI_ENONE;
	return space + 1;
}
