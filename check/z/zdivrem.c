/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

static void
checkdividebyzero(void)
{
	struct hebi_errstate es;
	struct hebi_error err;
	jmp_buf env;
	hebi_z q, r, a, b;
	int c;

	hebi_zinits(q, r, a, b, NULL);
	hebi_error_save(&es);
	
	switch (setjmp(env)) {
	case 0:
		hebi_error_jmp(env, 1);
		hebi_zdivrem(q, r, a, b);
		assert(!"no zero divided by zero raised");
		break;
	case 1:
		c = hebi_error_last(&err);
		assert(c == -1);
		assert(err.he_domain == HEBI_ERRDOM_HEBI);
		assert(err.he_code == HEBI_EZERODIVZERO);
		hebi_error_jmp(env, 2);
		hebi_zsetu(a, 10);
		hebi_zdivrem(q, r, a, b);
		assert(!"no divide by zero raised");
		break;
	case 2:
		c = hebi_error_last(&err);
		assert(c == -1);
		assert(err.he_domain == HEBI_ERRDOM_HEBI);
		assert(err.he_code == HEBI_EDIVZERO);
		break;
	default:
		assert(!"unexpected value returned from setjmp");
		break;
	}

	hebi_error_restore(&es);
	hebi_zdestroys(q, r, a, b, NULL);
}

static void
checkedgecases(void)
{
	hebi_z q, r, a, b;

	hebi_zinits(q, r, a, b, NULL);

	/*
	 * cases where the top of normalized dividend is larger or equal to
	 * the normalized divisor, triggering a correction to ensure the top of
	 * the dividend is less than the normalized divisor
	 */

	hebi_zsetstr(a, "0xFFAB15239CA381E3F1412D3301419807981490AB124908DE12490876BC912903", NULL, 0);
	hebi_zsetstr(b, "0x859486D4F84517298D4BD5C9EF209C88AA517F5561E6F9B5538798AB4EB8B779", NULL, 0);
	assert(hebi_zbits(a) == 256 && hebi_zbits(b) == 256);
	hebi_zdivrem(q, r, a, b);
	zcheckbc(q, "%Z / %Z", a, b);
	zcheckbc(r, "%Z %% %Z", a, b);

	hebi_zsetstr(a, "0xFE22F94210D832EAD173882718DEBF5F5C365EF6695D61298D0287B79146EA9B", NULL, 0);
	hebi_zsetstr(b, "0xFE22F94210D832EA7C869F37B225842AC624237A01FCB84306E6E0B831A093EA", NULL, 0);
	assert(hebi_zbits(a) == 256 && hebi_zbits(b) == 256);
	hebi_zdivrem(q, r, a, b);
	zcheckbc(q, "%Z / %Z", a, b);
	zcheckbc(r, "%Z %% %Z", a, b);

	hebi_zsetstr(a, "0xE09767DE6F2603B55C539A4C6A95CA413D0ACB69BF3179EE7026C52C587388E16B204988BDAA297B818D08B4C56130E4", NULL, 0);
	hebi_zsetstr(b, "0xE09767DE6F2603B55C539A4C6A95CA413D0ACB69BF3179EE7026C52C587388E1", NULL, 0);
	assert(hebi_zbits(a) == 384 && hebi_zbits(b) == 256);
	hebi_zdivrem(q, r, a, b);
	zcheckbc(q, "%Z / %Z", a, b);
	zcheckbc(r, "%Z %% %Z", a, b);

	/*
	 * rare cases where after a 3x2 reciprocal division pass, the top two
	 * limbs of the dividend are larger than the top two limbs of the
	 * divisor, triggering a special case estimation of the quotient limb
	 */

	/* TODO */

	hebi_zdestroys(q, r, a, b, NULL);
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	checkdividebyzero();
	checkedgecases();
	return 0;
}
