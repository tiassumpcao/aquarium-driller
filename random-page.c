#include <aquarium-driller/random.h>
#include <aquarium-driller/types.h>
#include <aquarium-driller/system.h>
#include <aquarium-driller/variable-generation.h>

static void create_pagesized_struct(char *page)
{
	size_t i;
	void **ptr;

	for (i = 0; i < PAGE_SIZE;) {
		ptr = (void *) &page[i];

		switch (RAND(sizeof(int)) % 4) {
		case 0:
			i += sizeof(unsigned int);
			if (i > PAGE_SIZE)
				return;
			*(unsigned int *) ptr = rand32();
			break;
		case 1:
			i += sizeof(unsigned long);
			if (i > PAGE_SIZE)
				return;
			*(unsigned long *) ptr = rand64();
			break;
		case 2:
			i += sizeof(void *);
			if (i > PAGE_SIZE)
				return;
			*ptr = get_address();
			break;
		case 3:
			i += sizeof(unsigned int);
			if (i > PAGE_SIZE)
				return;
			*(unsigned int *) ptr = RAND(sizeof(int)) % PAGE_SIZE;
			break;
		default:
			return;
		}
	}
}

void generate_random_page(char *page)
{
	size_t i;

	for (i = 0; i < PAGE_SIZE;) {
		page[i++] = RAND(sizeof(uint8_t));
	}
	return;

	switch (RAND(sizeof(int)) % 6) {
		/* return a page of complete trash */
	case 0:		/* bytes */
		for (i = 0; i < PAGE_SIZE;)
			page[i++] = RAND(sizeof(uint8_t));
		return;

	case 1:		/* words */
		for (i = 0; i < (PAGE_SIZE / 2);) {
			page[i++] = 0;
			page[i++] = RAND(sizeof(uint16_t));
		}
		return;

	case 2:		/* ints */
		for (i = 0; i < (PAGE_SIZE / 4);) {
			page[i++] = 0;
			page[i++] = 0;
			page[i++] = 0;
			page[i++] = RAND(sizeof(uint32_t));
		}
		return;

		/* return a page that looks kinda like a struct */
	case 3:
		create_pagesized_struct(page);
		return;

		/* page of 0's and 1's. */
	case 5:
		for (i = 0; i < PAGE_SIZE;)
			page[i++] = (unsigned char) rand_bool();
		return;

	default:
		return;
	}
}


