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
	puts("static const uint64_t logb2u64lut[64] = {");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 2 && (b+j) < 64; j++) {
			if (2 <= (b+j) && (b+j) <= 62) {
				logb2 = logl(2.0l) / logl(b+j);
				x = (uint64_t)(ceill(logb2 * 0x1p64l) - 1.0l);
			} else {
				x = 0;
			}
			printf("0x%016" PRIX64 "ULL", x);
			if ((b+j) != 63)
				printf(j == 1 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

static const uint64_t logb2u64lut[64] = {
	0x0000000000000000ULL, 0x0000000000000000ULL,
	0xFFFFFFFFFFFFFFFFULL, 0xA1849CC1A9A9E94EULL,
	0x7FFFFFFFFFFFFFFFULL, 0x6E40D1A4143DCB94ULL,
	0x6308C91B702A7CF4ULL, 0x5B3064EB3AA6D388ULL,
	0x5555555555555555ULL, 0x50C24E60D4D4F4A7ULL,
	0x4D104D427DE7FBCCULL, 0x4A00270775914E88ULL,
	0x4768CE0D05818E12ULL, 0x452E53E365907BDAULL,
	0x433CFFFB4B5AAE55ULL, 0x41867711B4F85354ULL,
	0x3FFFFFFFFFFFFFFFULL, 0x3EA16AFD58B10966ULL,
	0x3D64598D154DC4DEULL, 0x3C43C23018BB5562ULL,
	0x3B3B9A42873069C6ULL, 0x3A4898F06CF41AC9ULL,
	0x39680B13582E7C18ULL, 0x3897B2B751AE561AULL,
	0x37D5AED131F19C98ULL, 0x372068D20A1EE5CAULL,
	0x3676867E5D60DE29ULL, 0x35D6DEEB388DF86FULL,
	0x354071D61C77FA2EULL, 0x34B260C5671B18ACULL,
	0x342BE986572B45CCULL, 0x33AC61B998FBBDF2ULL,
	0x3333333333333333ULL, 0x32BFD90114C12861ULL,
	0x3251DCF6169E45F2ULL, 0x31E8D59F180DC630ULL,
	0x3184648DB8153E7AULL, 0x312434E89C35DACDULL,
	0x30C7FA349460A541ULL, 0x306F6F4C8432BC6DULL,
	0x301A557FFBFDD252ULL, 0x2FC873D1FDA55F3BULL,
	0x2F799652A4E6DC49ULL, 0x2F2D8D8F64460AADULL,
	0x2EE42E164E8F53A3ULL, 0x2E9D500984041DBDULL,
	0x2E58CEC05A6A8144ULL, 0x2E1688743EF9104CULL,
	0x2DD65DF7A583598FULL, 0x2D9832759D5369C4ULL,
	0x2D5BEB38DCD1394CULL, 0x2D216F7943E2BA6AULL,
	0x2CE8A82EFBB3FF2CULL, 0x2CB17FEA7AD7E332ULL,
	0x2C7BE2B0CFA1BA50ULL, 0x2C47BDDBA92D7463ULL,
	0x2C14FFFCAA8B131FULL, 0x2BE398C3A38BE053ULL,
	0x2BB378E758451068ULL, 0x2B8492108BE5E5F7ULL,
	0x2B56D6C70D55481BULL, 0x2B2A3A608C72DDD5ULL,
	0x2AFEB0F1060C7E41ULL, 0x0000000000000000ULL
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
	puts("static const uint32_t logb2u32lut[64] = {");
	for (b = 0, j = 0; b < 64; b += j) {
		putchar('\t');
		for (j = 0; j < 4 && (b+j) < 64; j++) {
			if (2 <= (b+j) && (b+j) <= 62) {
				logb2 = log(2) / log(b + j + 1);
				x = (uint32_t)(ceil(logb2 * 0x1p32) - 1.0);
			} else {
				x = 0;
			}
			printf("0x%08" PRIX32 "U", x);
			if ((b+j) != 63)
				printf(j == 3 ? "," : ", ");
		}
		putchar('\n');
	}
	puts("};");
	return 0;
}

#endif

static const uint32_t logb2u32lut[64] = {
	0x00000000U, 0x00000000U, 0xFFFFFFFFU, 0xA1849CC1U,
	0x7FFFFFFFU, 0x6E40D1A4U, 0x6308C91BU, 0x5B3064EBU,
	0x55555555U, 0x50C24E60U, 0x4D104D42U, 0x4A002707U,
	0x4768CE0DU, 0x452E53E3U, 0x433CFFFBU, 0x41867711U,
	0x3FFFFFFFU, 0x3EA16AFDU, 0x3D64598DU, 0x3C43C230U,
	0x3B3B9A42U, 0x3A4898F0U, 0x39680B13U, 0x3897B2B7U,
	0x37D5AED1U, 0x372068D2U, 0x3676867EU, 0x35D6DEEBU,
	0x354071D6U, 0x34B260C5U, 0x342BE986U, 0x33AC61B9U,
	0x33333333U, 0x32BFD901U, 0x3251DCF6U, 0x31E8D59FU,
	0x3184648DU, 0x312434E8U, 0x30C7FA34U, 0x306F6F4CU,
	0x301A557FU, 0x2FC873D1U, 0x2F799652U, 0x2F2D8D8FU,
	0x2EE42E16U, 0x2E9D5009U, 0x2E58CEC0U, 0x2E168874U,
	0x2DD65DF7U, 0x2D983275U, 0x2D5BEB38U, 0x2D216F79U,
	0x2CE8A82EU, 0x2CB17FEAU, 0x2C7BE2B0U, 0x2C47BDDBU,
	0x2C14FFFCU, 0x2BE398C3U, 0x2BB378E7U, 0x2B849210U,
	0x2B56D6C7U, 0x2B2A3A60U, 0x2AFEB0F1U, 0x00000000U
};

#endif /* SIZE_MAX > UINT32_MAX */

HEBI_API
size_t
hebi_pgetstrlen(const hebi_packet *a, size_t n, int base)
{
	const unsigned int flags = (unsigned int)base;
	const unsigned int ubase = flags & HEBI_STR_BASEMASK;

	size_t bits;
	size_t len;
	size_t log2b;
#if SIZE_MAX > UINT32_MAX
	uint64_t lenlo;
	uint64_t lenhi;
	uint64_t logb2;
#else
	uint32_t logb2;
#endif

	ASSERT(n <= HEBI_PACKET_MAXLEN);
	ASSERT(2 <= ubase && ubase <= 62);

	if (LIKELY(n > 0)) {
		bits = n * HEBI_PACKET_BIT - hebi_pclz(a, n);
		if ((ubase & (ubase - 1))) {
#if SIZE_MAX > UINT32_MAX
			logb2 = logb2u64lut[ubase];
			hebi_umad128__(&lenlo, &lenhi, logb2, bits, bits);
			len = (size_t)lenhi;
#else
			logb2 = logb2u32lut[ubase];
			len = (size_t)(((uint64_t)logb2 * bits + bits) >> 32);
#endif
			len++;
		} else {
			log2b = hebi_floorlog2sz__(ubase);
			len = (bits + log2b - 1) / log2b;
		}
	} else {
		len = 1;
	}

	if (flags & HEBI_STR_PREFIX) {
		if (ubase == 2 || ubase == 16)
			len += 2;
		else if (ubase == 8 && n > 0)
			len += 1;
	}

	return len;
}
