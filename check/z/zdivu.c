#include "../check.h"

static void
checkdividebyzero(void)
{
	struct hebi_errstate es;
	struct hebi_error err;
	jmp_buf env;
	hebi_z a;
	int c, v;

	hebi_zinit(a);
	hebi_error_save(&es);
	
	if (!(v = setjmp(env))) {
		hebi_error_jmp(env, 1);
		hebi_zdivu(a, a, 0);
		fail("no zero divided by zero raised");
	} else if (v == 1) {
		c = hebi_error_last(&err);
		assert(c == -1);
		assert(err.he_domain == HEBI_ERRDOM_HEBI);
		assert(err.he_code == HEBI_EZERODIVZERO);
		hebi_error_jmp(env, 2);
		hebi_zsetu(a, 10);
		hebi_zdivu(a, a, 0);
		fail("no divide by zero raised");
	} else if (v != 2) {
		fail("unexpected value returned from setjmp");
	}

	c = hebi_error_last(&err);
	assert(c == -1);
	assert(err.he_domain == HEBI_ERRDOM_HEBI);
	assert(err.he_code == HEBI_EDIVZERO);

	hebi_error_restore(&es);
	hebi_zdestroy(a);
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopu64(hebi_zdivu, "%Z / %llu", RHS_NONZERO);
	checkdividebyzero();
	return 0;
}
