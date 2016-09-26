#include "../check.h"

int main(int argc, char *argv[])
{
	const int64_t d = INT64_C(3484957213536676883);
	hebi_z a, b, c, two;
	int i;

	hebi_zinits(a, b, c, two, NULL);

	hebi_zshr(c, a, 0);
	assert(hebi_zcapacity(c) == 0 && hebi_zused(c) == 0);

	hebi_zshr(c, a, 325);
	assert(hebi_zcapacity(c) == 0 && hebi_zused(c) == 0);

	hebi_zsetu(two, 2);
	hebi_zseti(c, d);
	hebi_zshl(c, c, 2048);
	hebi_zseti(b, d);
	hebi_zshl(b, b, 2048);

	for (i = 1; i < 2048; ++i) {
		zdirty(a, c, NULL);
		hebi_zshr(a, c, i);
		hebi_zdivu(b, b, 2);
		assert(!hebi_zcmp(a, b));
	}

	hebi_zdestroys(a, b, c, two, NULL);
	return 0;
}
