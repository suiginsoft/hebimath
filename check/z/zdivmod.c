#include "check.h"
#include <stdio.h>

static const char a1[] =
"20370359763344860862684456884093781610514683"
"9366593625063614044935499763336676180097353";

static const char b1[] =
"11579208923731619542357098500868790785326998"
"4665640564039457584007913129639935";

// a1 / b1
static const char q1[] = "1759218604";

char abuf[8192];
char bbuf[8192];
char qbuf[8192];
char rbuf[8192];

int main(int argc, char *argv[])
{
	check_options opt = { .iter = 10, .an = 32, .bn = 32 };
	hebi_z a, b, q, r, s, t;
	size_t an, bn;
	int i;

	check_init(argc, argv, &opt);
	hebi_zinits(a, b, q, r, s, t, NULL);

	assert(hebi_zdivmod(q, NULL, a, b) == hebi_divzero);

	assert(hebi_zsetstr(a, a1, NULL, 0) == hebi_success);
	assert(hebi_zdivmod(q, NULL, a, b) == hebi_divzero);

	assert(hebi_zsetstr(b, b1, NULL, 0) == hebi_success);
	assert(hebi_zsetstr(s, q1, NULL, 0) == hebi_success);
	assert(hebi_zdivmod(q, NULL, a, b) == hebi_success);
	assert(!hebi_zcmp(q, s));

	/* stress test */
	for (i = 0; i < opt.iter; i++) {
		for (an = 1; an <= opt.an; an++) {
			hebi_zrand_kiss(a, an, &check_kiss);
			hebi_zgetstr(abuf, sizeof(abuf), a, 10);
			for (bn = 1; bn <= opt.bn; bn++) {
				hebi_zrand_kiss(b, bn, &check_kiss);
				hebi_zgetstr(bbuf, sizeof(bbuf), b, 10);

				assert(hebi_zdivmod(q, r, a, b) == hebi_success);
				hebi_zgetstr(qbuf, sizeof(qbuf), q, 10);
				hebi_zgetstr(rbuf, sizeof(rbuf), r, 10);

				assert(check_bc(qbuf, "%s / %s", abuf, bbuf));
				assert(check_bc(rbuf, "%s %% %s", abuf, bbuf));
			}
		}
	}

	hebi_zdestroys(a, b, q, r, s, t, NULL);
	return 0;
}
