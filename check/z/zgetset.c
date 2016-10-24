/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"
#include <string.h>

static const char s0[] = "0";

static const char s1[] =
"61180309865469664074115714709070871077932745749907283107141473754384";

static const char s2[] =
"-8120396648443974081224497212774464476732599148753804434410127854136"
"41400430006471188254672352726201340735972467249908384163750955495710"
"34343511803098654696646524364363246077932745749907283107141473754385";

int main(int argc, char *argv[])
{
	char buffer[2048];
	hebi_z a;

	hebi_zinit(a);
	assert(hebi_zsign(a) == 0);
	assert(hebi_zused(a) == 0);
	assert(hebi_zeven(a));
	assert(!hebi_zodd(a));
	assert(hebi_zzero(a));
	assert(hebi_zgeti(a) == 0);
	assert(hebi_zgetu(a) == 0);
	assert(hebi_zgetsi(a) == 0);
	assert(hebi_zgetsu(a) == 0);

	hebi_zseti(a, 123456789);
	assert(hebi_zsign(a) > 0);
	assert(hebi_zused(a) == 1);
	assert(hebi_zcapacity(a) == 1);
	assert(!hebi_zeven(a));
	assert(hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgeti(a) == 123456789);
	assert(hebi_zgetu(a) == 123456789);
	assert(hebi_zgetsi(a) == 123456789);
	assert(hebi_zgetsu(a) == 123456789);

	hebi_zseti(a, INT64_MAX - 1);
	assert(hebi_zsign(a) > 0);
	assert(hebi_zused(a) == 1);
	assert(hebi_zcapacity(a) == 1);
	assert(hebi_zeven(a));
	assert(!hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgetsi(a) == INT64_MAX - 1);
	assert(hebi_zgetsu(a) == INT64_MAX - 1);

	hebi_zseti(a, INT64_MIN + 1);
	assert(hebi_zsign(a) < 0);
	assert(hebi_zused(a) == 1);
	assert(hebi_zcapacity(a) == 1);
	assert(!hebi_zeven(a));
	assert(hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgetsi(a) == INT64_MIN + 1);
	assert(hebi_zgetsu(a) == 0);

	hebi_zsetu(a, UINT64_MAX - 1);
	assert(hebi_zsign(a) > 0);
	assert(hebi_zused(a) == 1);
	assert(hebi_zcapacity(a) == 1);
	assert(hebi_zeven(a));
	assert(!hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgetsu(a) == UINT64_MAX - 1);
	assert(hebi_zgetsi(a) == INT64_MAX);

	hebi_zseti(a, 0);
	assert(hebi_zsign(a) == 0);
	assert(hebi_zused(a) == 0);
	assert(hebi_zcapacity(a) == 1);
	assert(hebi_zeven(a));
	assert(!hebi_zodd(a));
	assert(hebi_zzero(a));
	assert(hebi_zgeti(a) == 0);
	assert(hebi_zgetu(a) == 0);
	assert(hebi_zgetsi(a) == 0);
	assert(hebi_zgetsu(a) == 0);
	assert(hebi_zgetstr(buffer, sizeof(buffer), a, 10) < sizeof(buffer));
	assert(strcmp(buffer, s0) == 0);

	assert(hebi_zsetstr(a, s1, NULL, 10) > 0);
	assert(hebi_zsign(a) > 0);
	assert(hebi_zused(a) == 2);
	assert(hebi_zeven(a));
	assert(!hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgetsi(a) == INT64_MAX);
	assert(hebi_zgetsu(a) == UINT64_MAX);
	assert(hebi_zgetstr(buffer, sizeof(buffer), a, 10) < sizeof(buffer));
	assert(strcmp(buffer, s1) == 0);

	assert(hebi_zsetstr(a, s2, NULL, 0) > 0);
	assert(hebi_zsign(a) < 0);
	assert(hebi_zused(a) == 6);
	assert(!hebi_zeven(a));
	assert(hebi_zodd(a));
	assert(!hebi_zzero(a));
	assert(hebi_zgetsi(a) == INT64_MIN);
	assert(hebi_zgetsu(a) == 0);
	assert(hebi_zgetstr(buffer, sizeof(buffer), a, 10) < sizeof(buffer));
	assert(strcmp(buffer, s2) == 0);

	hebi_zdestroy(a);

	return 0;
}
