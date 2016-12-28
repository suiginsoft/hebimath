/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "pcommon.h"

#if SIZE_MAX > UINT32_MAX

/*
 * logb2u64lut is lookup-table representing a 64-bit fixed-point approximation
 * of log(2)/log(b) used for logarithmic base conversion. It is generated with
 * the following program:
 */

#if 0

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	long double logb2;
	uint64_t x;
	int b, j;
	puts("static const uint64_t logb2u64lut[64] =");
	puts("{");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 2 && (b+j) < 64; j++) {
			if (2 <= (b+j) && (b+j) <= 62) {
				logb2 = logl(2.0l) / logl(b+j);
				x = (uint64_t)(ceill(logb2 * 0x1p64l) - 1.0l);
			} else {
				x = 0;
			}
			printf("0x%016" PRIX64 "ull", x);
			if ((b+j) != 63)
				printf(j == 1 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

static const uint64_t logb2u64lut[64] =
{
	0x0000000000000000ull, 0x0000000000000000ull,
	0xFFFFFFFFFFFFFFFFull, 0xA1849CC1A9A9E94Eull,
	0x7FFFFFFFFFFFFFFFull, 0x6E40D1A4143DCB94ull,
	0x6308C91B702A7CF4ull, 0x5B3064EB3AA6D388ull,
	0x5555555555555555ull, 0x50C24E60D4D4F4A7ull,
	0x4D104D427DE7FBCCull, 0x4A00270775914E88ull,
	0x4768CE0D05818E12ull, 0x452E53E365907BDAull,
	0x433CFFFB4B5AAE55ull, 0x41867711B4F85354ull,
	0x3FFFFFFFFFFFFFFFull, 0x3EA16AFD58B10966ull,
	0x3D64598D154DC4DEull, 0x3C43C23018BB5562ull,
	0x3B3B9A42873069C6ull, 0x3A4898F06CF41AC9ull,
	0x39680B13582E7C18ull, 0x3897B2B751AE561Aull,
	0x37D5AED131F19C98ull, 0x372068D20A1EE5CAull,
	0x3676867E5D60DE29ull, 0x35D6DEEB388DF86Full,
	0x354071D61C77FA2Eull, 0x34B260C5671B18ACull,
	0x342BE986572B45CCull, 0x33AC61B998FBBDF2ull,
	0x3333333333333333ull, 0x32BFD90114C12861ull,
	0x3251DCF6169E45F2ull, 0x31E8D59F180DC630ull,
	0x3184648DB8153E7Aull, 0x312434E89C35DACDull,
	0x30C7FA349460A541ull, 0x306F6F4C8432BC6Dull,
	0x301A557FFBFDD252ull, 0x2FC873D1FDA55F3Bull,
	0x2F799652A4E6DC49ull, 0x2F2D8D8F64460AADull,
	0x2EE42E164E8F53A3ull, 0x2E9D500984041DBDull,
	0x2E58CEC05A6A8144ull, 0x2E1688743EF9104Cull,
	0x2DD65DF7A583598Full, 0x2D9832759D5369C4ull,
	0x2D5BEB38DCD1394Cull, 0x2D216F7943E2BA6Aull,
	0x2CE8A82EFBB3FF2Cull, 0x2CB17FEA7AD7E332ull,
	0x2C7BE2B0CFA1BA50ull, 0x2C47BDDBA92D7463ull,
	0x2C14FFFCAA8B131Full, 0x2BE398C3A38BE053ull,
	0x2BB378E758451068ull, 0x2B8492108BE5E5F7ull,
	0x2B56D6C70D55481Bull, 0x2B2A3A608C72DDD5ull,
	0x2AFEB0F1060C7E41ull, 0x0000000000000000ull
};

#else /* SIZE_MAX > UINT32_MAX */

/*
 * logb2u32lut is lookup-table representing a 32-bit fixed-point approximation
 * of log(2)/log(b) used for logarithmic base conversion. It is generated with
 * the following program:
 */

#if 0

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	double logb2;
	uint32_t x;
	int b, j;
	puts("static const uint32_t logb2u32lut[64] =");
	puts("{");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 4 && (b+j) < 64; j++) {
			if (2 <= (b+j) && (b+j) <= 62) {
				logb2 = log(2) / log(b + j + 1);
				x = (uint32_t)(ceil(logb2 * 0x1p32) - 1.0);
			} else {
				x = 0;
			}
			printf("0x%08" PRIX32 "u", x);
			if ((b+j) != 63)
				printf(j == 3 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

static const uint32_t logb2u32lut[64] =
{
	0x00000000u, 0x00000000u, 0xFFFFFFFFu, 0xA1849CC1u,
	0x7FFFFFFFu, 0x6E40D1A4u, 0x6308C91Bu, 0x5B3064EBu,
	0x55555555u, 0x50C24E60u, 0x4D104D42u, 0x4A002707u,
	0x4768CE0Du, 0x452E53E3u, 0x433CFFFBu, 0x41867711u,
	0x3FFFFFFFu, 0x3EA16AFDu, 0x3D64598Du, 0x3C43C230u,
	0x3B3B9A42u, 0x3A4898F0u, 0x39680B13u, 0x3897B2B7u,
	0x37D5AED1u, 0x372068D2u, 0x3676867Eu, 0x35D6DEEBu,
	0x354071D6u, 0x34B260C5u, 0x342BE986u, 0x33AC61B9u,
	0x33333333u, 0x32BFD901u, 0x3251DCF6u, 0x31E8D59Fu,
	0x3184648Du, 0x312434E8u, 0x30C7FA34u, 0x306F6F4Cu,
	0x301A557Fu, 0x2FC873D1u, 0x2F799652u, 0x2F2D8D8Fu,
	0x2EE42E16u, 0x2E9D5009u, 0x2E58CEC0u, 0x2E168874u,
	0x2DD65DF7u, 0x2D983275u, 0x2D5BEB38u, 0x2D216F79u,
	0x2CE8A82Eu, 0x2CB17FEAu, 0x2C7BE2B0u, 0x2C47BDDBu,
	0x2C14FFFCu, 0x2BE398C3u, 0x2BB378E7u, 0x2B849210u,
	0x2B56D6C7u, 0x2B2A3A60u, 0x2AFEB0F1u, 0x00000000u
};

#endif /* SIZE_MAX > UINT32_MAX */

HEBI_API
size_t
hebi_pgetstrlen(const hebi_packet *a, size_t n, int base)
{
	int flags;
	size_t len, log2b, nbits;
#if SIZE_MAX > UINT32_MAX
	uint64_t lenlo, lenhi, logb2;
#else
	uint32_t logb2;
#endif

	flags = base;
	base = base & HEBI_STR_BASEMASK;

	ASSERT(0 < n && n <= HEBI_PACKET_MAXLEN);
	ASSERT(2 <= base && base <= 62);

	nbits = n * HEBI_PACKET_BIT - hebi_pclz(a, n);

	if ((base & (base - 1)) != 0) {
#if SIZE_MAX > UINT32_MAX
		logb2 = logb2u64lut[base];
		hebi_umad128__(&lenlo, &lenhi, logb2, nbits, nbits);
		len = (size_t)lenhi;
#else
		logb2 = logb2u32lut[base];
		len = (size_t)(((uint64_t)logb2 * nbits + nbits) >> 32);
#endif
		++len;
	} else {
		log2b = hebi_floorlog2sz__((size_t)base);
		len = (nbits + log2b - 1) / log2b;
	}

	if (flags & HEBI_STR_PREFIX) {
		if (base == 2 || base == 16)
			len += 2;
		else if (base == 8)
			len += 1;
	}

	return len;
}
