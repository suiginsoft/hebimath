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
		(void)hebi_zremu(a, 0);
		assert(!"no zero divided by zero raised");
	} else if (v == 1) {
		c = hebi_error_last(&err);
		assert(c == -1);
		assert(err.he_domain == HEBI_ERRDOM_HEBI);
		assert(err.he_code == HEBI_EZERODIVZERO);
		hebi_error_jmp(env, 2);
		hebi_zsetu(a, 10);
		(void)hebi_zremu(a, 0);
		assert(!"no divide by zero raised");
	} else if (v != 2) {
		assert(!"unexpected value returned from setjmp");
	}

	c = hebi_error_last(&err);
	assert(c == -1);
	assert(err.he_domain == HEBI_ERRDOM_HEBI);
	assert(err.he_code == HEBI_EDIVZERO);

	hebi_error_restore(&es);
	hebi_zdestroy(a);
}

static void
zremu(hebi_zptr r, hebi_zsrcptr a, uint64_t b)
{
	int s = hebi_zsign(a);
	hebi_zsetu(r, hebi_zremu(a, b));
	if (s < 0)
		hebi_zneg(r, r);
}

int
main(int argc, char *argv[])
{
	checkinit(argc, argv);
	zcheckbinopu64(zremu, "%Z %% %llu", RHS_NONZERO);
	checkdividebyzero();
	return 0;
}
