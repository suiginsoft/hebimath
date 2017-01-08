/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

int
main(int argc, char *argv[])
{
	char fixedbuf[80];
	void* p;

	const struct hebi_allocfnptrs *fp;
	struct hebi_errstate es;
	struct hebi_error err;
	jmp_buf env;
	int i;

	fp = hebi_alloc_query(NULL, HEBI_ALLOC_FIXED);
	assert(fp && fp->ha_alloc && fp->ha_free);

	hebi_error_save(&es);

	p = NULL;
	i = setjmp(env);

	if (i > 0) {
		assert(hebi_error_last(&err));
		assert(err.he_domain == HEBI_ERRDOM_HEBI && err.he_code == HEBI_ENOMEM);
	}

	if (i == 0) {
		hebi_error_jmp(env, 1);
		hebi_freefp(fp, NULL, 0);
		hebi_freefp(fp, fixedbuf, sizeof(fixedbuf));
		p = hebi_allocfp(fp, 8, 32);
		assert(!"allocating with HEBI_ALLOC_FIXED didn't raise an error");
	} else if (i == 1) {
		hebi_error_jmp(env, 2);
		hebi_free(HEBI_ALLOC_FIXED, NULL, 0);
		hebi_free(HEBI_ALLOC_FIXED, fixedbuf, sizeof(fixedbuf));
		p = hebi_alloc(HEBI_ALLOC_FIXED, 16, 64);
		assert(!"allocating with HEBI_ALLOC_FIXED didn't raise an error");
	}

	assert(i == 2);
	assert(p == NULL);

	hebi_error_restore(&es);

	(void)argc;
	(void)argv;
	return 0;
}
