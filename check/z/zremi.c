#include "../check.h"

int
main(int argc, char *argv[])
{
	hebi_z r, a;
	int64_t b;

	checkinit(argc, argv);
	hebi_zinits(r, a, NULL);

	if (check_max_perm <= 0)
		check_max_perm = 256;
	check_max_iter = check_max_perm;

	for (check_iter = check_start_iter;
			check_iter < check_max_iter;
			check_iter++) {
		zpermutation(check_iter, check_max_perm, 1, a);
		for (check_pass = 0; check_pass < check_num_i64values; check_pass++) {
			b = check_i64values[check_pass];
			if (!b)
				continue;
			hebi_zseti(r, hebi_zremi(a, b));
			zcheckbc(r, "%Z %% %lld", a, (long long)b);
		}
	}

	hebi_zdestroys(r, a, NULL);
	return 0;
}
