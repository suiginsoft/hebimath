#include "../check.h"
#include <stdio.h>

int
main(int argc, char *argv[])
{
	const int64_t d = INT64_C(3484957213536676883);
	hebi_z a, b, c, two;
	int i;

	hebi_zinits(a, b, c, two, NULL);

	hebi_zshl(c, a, 0);
	assert(hebi_zcapacity(c) == 0 && hebi_zused(c) == 0);

	hebi_zshl(c, a, 325);
	assert(hebi_zcapacity(c) == 0 && hebi_zused(c) == 0);

	hebi_zseti(c, d);
	hebi_zseti(b, d);
	hebi_zsetu(two, 2);

	for (i = 1; i < 2048; ++i) {
		zdirty(a, c, NULL);
		hebi_zshl(a, c, i);
		hebi_zmul(b, b, two);
		assert(!hebi_zcmp(a, b));
	}

	hebi_zseti(a, 1);
	for (i = 0; i < HEBI_PACKET_BIT; ++i) {
		assert(hebi_zbits(a) == i + 1);
		zdirty(a, NULL);
		hebi_zshl(a, a, 1);
	}
	assert(hebi_zbits(a) == HEBI_PACKET_BIT + 1);

	hebi_zdestroys(a, b, c, two, NULL);
	return 0;
}
