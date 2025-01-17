#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <aquarium-driller/system.h>
#include <aquarium-driller/random.h>
#include <aquarium-driller/variable-generation.h>
#include <aquarium-driller/memory-maps.h>


void simple_function (uint32_t interesting_uint32, uint64_t interesting_uint64,
		uint64_t arch_interesting_addr, uint64_t simple_type)
{
	printf ("Interesting 32-bit value :: %"PRIx32"\n", interesting_uint32);
	printf ("Interesting 64-bit value :: %"PRIx64"\n", interesting_uint64);
	printf ("Arch interesting address :: %"PRIx64"\n", arch_interesting_addr);
	printf ("Simple type :: %"PRIx64"\n", simple_type);
}

void complex_function (uint32_t interesting_uint32, uint64_t interesting_uint64,
		uint64_t arch_interesting_addr, uint64_t complex_type)
{
	printf ("Interesting 32-bit value :: %"PRIx32"\n", interesting_uint32);
	printf ("Interesting 64-bit value :: %"PRIx64"\n", interesting_uint64);
	printf ("Arch interesting address :: %"PRIx64"\n", arch_interesting_addr);
	printf ("Complex type :: %"PRIx64"\n", complex_type);
}

int main (int argc, char ** argv)
{
	unsigned long	lowval = 0;
	int				complex_type = 0;
	int				simple_type = 0;
	int				simple_type_counter = 0;
	size_t			iteration = 0;
	struct map		*map = NULL;
	char			*ptr = NULL;
	void			**allocs = NULL;

	if (argc != 2) {
		printf ("%s <per-arch low-value>\n", argv[0]);
		exit (1);
	}

	lowval = strtoul (argv[1], NULL, 16);

	AD_INIT_DRILLER(0xdeadbeefbabebeef);

	for (iteration = 0, simple_type_counter = 0; simple_type_counter < SIMPL_TYPE;
			simple_type = 1 << simple_type_counter, simple_type_counter++) {
		printf (" ==== SIMPLE TYPE = %s | ITERATION #%zu ====\n",
				simple_arg_names[simple_type], iteration);

		simple_function (AD_INTERESTING_32BIT_VALUE, AD_INTERESTING_64BIT_VALUE,
				AD_PER_ARCH_INTERESTING_ADDR(lowval), AD_SIMPLE_TYPE(simple_type));

		reseed ();
		iteration++;
	}

	for (complex_type = 0; complex_type < COMPL_TYPE; complex_type++) {
		printf (" ==== COMPLEX TYPE = %s | ITERATION #%zu ====\n",
				complex_arg_names[complex_type], iteration);

		complex_function (AD_INTERESTING_32BIT_VALUE, AD_INTERESTING_64BIT_VALUE,
				AD_PER_ARCH_INTERESTING_ADDR(lowval), AD_COMPLEX_TYPE(complex_type));

		reseed ();
		iteration++;
	}

	for (iteration = 0; iteration < 20; iteration++) {
		uint32_t u32;
		uint64_t u64;

		printf("=============================================================================\n");
		printf("rand_bool> %X\n", rand_bool());
		printf("rand_single_bit> %X\n", rand_single_bit(1));
		printf("randbits> %X\n", (int) randbits(12));
		printf("t4v1s0> %X\n", (int) t4v1s0());
		printf("rand8x8> %X\n", (int) rand8x8());
		printf("rept8> %X\n", (int) rept8(17));
		printf("__rand32> %X\n", __rand32());
		printf("rand32> %X\n", rand32());
		printf("RAND 8> %"PRIx64"\n", RAND(sizeof(uint8_t)));
		printf("RAND 8> %"PRIx64"\n", RAND(sizeof(uint8_t)));
		printf("RAND 8> %"PRIx64"\n", RAND(sizeof(uint8_t)));
		printf("RAND 8> %"PRIx64"\n", RAND(sizeof(uint8_t)));
		get_random_bytes(&u32, sizeof(uint32_t));
		printf("get_random_bytes 32> %"PRIX32"\n", u32);
		get_random_bytes(&u32, sizeof(uint32_t));
		printf("get_random_bytes 32> %"PRIX32"\n", u32);
		get_random_bytes(&u32, sizeof(uint32_t));
		printf("get_random_bytes 32> %"PRIX32"\n", u32);
		get_random_bytes(&u32, sizeof(uint32_t));
		printf("get_random_bytes 32> %"PRIX32"\n", u32);
		get_random_bytes(&u32, sizeof(uint32_t));
		printf("get_random_bytes 32> %"PRIX32"\n", u32);

		get_random_bytes(&u64, sizeof(uint64_t));
		printf("get_random_bytes 64> %"PRIX64"\n", u64);
	}

	printf("\n[WORKING SET :: %zu MAPS :: %d BUFFERS]\n\n", get_memory_maps_size(), 4);
	printf("- MEMORY BUFFERS -\n");
	ptr = get_page_zero();
	printf("\nName: page zeros\n");
	printf("Address: %p\n", ptr);
	printf("Size: %d\n", PAGE_ZEROS_SIZE);
	dump_memory_range(ptr, PAGE_ZEROS_SIZE, true);

	ptr = get_page_0xff();
	printf("\nName: page 0xFFs\n");
	printf("Address: %p\n", ptr);
	printf("Size: %d\n", PAGE_FF_SIZE);
	dump_memory_range(ptr, PAGE_FF_SIZE, true);

	ptr = get_page_rand();
	printf("\nName: page random\n");
	printf("Address: %p\n", ptr);
	printf("Size: %d\n", PAGE_RAND_SIZE);
	dump_memory_range(ptr, PAGE_RAND_SIZE, true);

	allocs = get_page_allocs();
	printf("\nName: adjacent page allocations\n");

	for (iteration = 0; iteration < (PAGE_ALLOC_SIZE / sizeof(void *)); iteration ++) {
		printf("Index: %zu\n", iteration);
		printf("Address: %p\n", allocs[iteration]);
		printf("Size: %d\n", PAGE_SIZE);
		dump_memory_range(allocs[iteration], PAGE_SIZE, true);
	}

	printf("\n- MEMORY MAPS -\n");
	map = get_first_map();
	while (map) {
		if (map->ptr == NULL) {
			map = map->next;
			continue;
		}
		printf("\nName: %s\n", map->name);
		printf("Address: %p\n", map->ptr);
		printf("Size: %zu\n", map->size);

		dump_memory_range(map->ptr, map->size, true);
		map = map->next;
	}

	AD_EXIT_DRILLER;
	return 0;
}


