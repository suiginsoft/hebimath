/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "../check.h"

enum { MAX_ITERATIONS = 256 };
enum { MAX_ALLOCATORS = 1024 };

static hebi_allocid allocators[MAX_ALLOCATORS];
static hebi_allocid previous_allocators[MAX_ALLOCATORS];
static int allocation_counts[MAX_ALLOCATORS];
static hebi_packet allocation_packets[MAX_ALLOCATORS];

static void *
customalloc(void *arg, size_t alignment, size_t size)
{
	assert((uintptr_t)arg < MAX_ALLOCATORS);
	assert(alignment == HEBI_PACKET_ALIGNMENT);
	assert(size == sizeof(hebi_packet));
	allocation_counts[(uintptr_t)arg]++;
	return &allocation_packets[(uintptr_t)arg];
}

static void
customfree(void *arg, void *ptr, size_t size)
{
	assert((uintptr_t)arg < MAX_ALLOCATORS);
	assert(ptr == &allocation_packets[(uintptr_t)arg]);
	assert(size == sizeof(hebi_packet));
	allocation_counts[(uintptr_t)arg]--;
}

int
main(int argc, char *argv[])
{
	hebi_allocid id;
	const struct hebi_allocfnptrs *fp;
	struct hebi_allocfnptrs fnptrs;
	size_t iteration;
	hebi_packet *p;
	size_t i;

	for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		for (i = 0; i < MAX_ALLOCATORS; i++) {
			fnptrs.ha_alloc = &customalloc;
			fnptrs.ha_free = &customfree;
			fnptrs.ha_arg = (void *)(uintptr_t)i;
			allocators[i] = hebi_alloc_add(&fnptrs);
			assert(allocators[i] > 0);
			assert(!iteration || allocators[i] != previous_allocators[i]);
		}

		for (i = 0; i < MAX_ALLOCATORS; i++) {
			assert(hebi_alloc_valid(allocators[i]));
			assert(!iteration || !hebi_alloc_valid(previous_allocators[i]));
		}

		for (i = 0; i < MAX_ALLOCATORS; i++) {
			assert(allocation_counts[i] == 0);
			p = hebi_alloc(allocators[i], HEBI_PACKET_ALIGNMENT, sizeof(hebi_packet));
			assert(allocation_counts[i] == 1);
			assert(p == &allocation_packets[i]);
		}

		for (i = MAX_ALLOCATORS; i > 0; i--) {
			assert(allocation_counts[i-1] == 1);

			fp = hebi_alloc_query(&id, allocators[i-1]);
			assert(id == allocators[i-1]);
			assert(fp && fp->ha_alloc == &customalloc &&
					fp->ha_free == &customfree &&
					fp->ha_arg == (void *)(uintptr_t)(i-1));

			p = hebi_allocfp(fp, HEBI_PACKET_ALIGNMENT, sizeof(hebi_packet));
			assert(allocation_counts[i-1] == 2);
			assert(p == &allocation_packets[i-1]);
		}

		for (i = 0; i < MAX_ALLOCATORS; i++) {
			assert(allocation_counts[i] == 2);

			fp = hebi_alloc_query(&id, allocators[i]);
			assert(id == allocators[i]);
			assert(fp && fp->ha_alloc == &customalloc &&
					fp->ha_free == &customfree &&
					fp->ha_arg == (void *)(uintptr_t)i);

			hebi_freefp(fp, &allocation_packets[i], sizeof(hebi_packet));
			assert(allocation_counts[i] == 1);
		}

		for (i = MAX_ALLOCATORS; i > 0; i--) {
			assert(allocation_counts[i-1] == 1);
			hebi_free(allocators[i-1], &allocation_packets[i-1], sizeof(hebi_packet));
			assert(allocation_counts[i-1] == 0);
		}

		for (i = 0; i < MAX_ALLOCATORS; i++) {
			previous_allocators[i] = allocators[i];
			hebi_alloc_remove(allocators[i]);
			assert(!hebi_alloc_valid(allocators[i]));
		}
	}

	(void)argc;
	(void)argv;
	return 0;
}
