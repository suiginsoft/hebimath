/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"
#include <limits.h>

static void
testvalid(void)
{
	const struct hebi_allocfnptrs *fp;
	const struct hebi_allocfnptrs *sfp;
	const struct hebi_allocfnptrs *dfp;
	const struct hebi_allocfnptrs *ffp;
	hebi_allocid id;

	fp = hebi_alloc_query(NULL, HEBI_ALLOC_STDLIB);
	assert(fp && fp->ha_alloc && fp->ha_free);

	dfp = hebi_alloc_query(NULL, HEBI_ALLOC_DEFAULT);
	sfp = hebi_alloc_query(NULL, HEBI_ALLOC_SCRATCH);
	assert(dfp && sfp && fp == dfp && dfp == sfp);

	ffp = hebi_alloc_query(NULL, HEBI_ALLOC_FIXED);
	assert(ffp && ffp != fp && ffp->ha_alloc && ffp->ha_free);

	fp = hebi_alloc_query(&id, HEBI_ALLOC_STDLIB);
	assert(fp == dfp && id == HEBI_ALLOC_STDLIB);

	dfp = hebi_alloc_query(&id, HEBI_ALLOC_DEFAULT);
	assert(fp == dfp && id == HEBI_ALLOC_STDLIB);

	sfp = hebi_alloc_query(&id, HEBI_ALLOC_SCRATCH);
	assert(fp == sfp && id == HEBI_ALLOC_STDLIB);

	ffp = hebi_alloc_query(&id, HEBI_ALLOC_FIXED);
	assert(ffp && ffp != fp && id == HEBI_ALLOC_FIXED);
}

static void
testinvalid(void)
{
	enum { NUM_INVALID = 8 };

	static const hebi_allocid invalid_ids[NUM_INVALID] = {
		HEBI_ALLOC_INVALID,
		INT_MIN,
		INT_MAX,
		1,
		2,
		16384,
		-25,
		-28415
	};

	struct hebi_errstate es;
	struct hebi_error err;
	hebi_allocid id;
	jmp_buf env;
	int i;

	hebi_error_save(&es);

	i = setjmp(env);

	if (i > 0) {
		assert(hebi_error_last(&err));
		assert(err.he_domain == HEBI_ERRDOM_HEBI && err.he_code == HEBI_EBADVALUE);
	}

	if (i >= 0 && i < NUM_INVALID) {
		hebi_error_jmp(env, i + 1);
		(void)hebi_alloc_query(&id, invalid_ids[(size_t)i]);
		assert(!"hebi_alloc_query didn't raise an error");
	}

	assert(i == NUM_INVALID);

	hebi_error_restore(&es);
}

int
main(int argc, char *argv[])
{
	testvalid();
	testinvalid();
	(void)argc;
	(void)argv;
	return 0;
}
