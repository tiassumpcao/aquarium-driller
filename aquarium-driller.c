#include <stdio.h>
#include <ctype.h>
#include <aquarium-driller/system.h>
#include <aquarium-driller/random.h>
 
#define HEXDUMP_COLS 8
#define TRUNCATION_LENGTH 256

bool ad_fail;

void dump_memory_range(void *memory, size_t length, unsigned char truncate)
{
	unsigned int	i,
					j;

	if (truncate == true)
		length = TRUNCATION_LENGTH;
	for (i = 0; i < length + ((length % HEXDUMP_COLS) ? (HEXDUMP_COLS - length % HEXDUMP_COLS) : 0); i++) {
		if ((i % HEXDUMP_COLS) == 0)
			printf("%p:%06X: ", memory, i);
		if (i < length)
			printf("%02X ", 0xff & ((char *) memory)[i]);
		else
			printf("   ");

		if ((i % HEXDUMP_COLS) == (HEXDUMP_COLS - 1)) {
			for (j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
				if(j >= length)
					putchar(' ');
				else if (isprint(((char *) memory)[j]))
					putchar(0xff & ((char *) memory)[j]); 
				else
					putchar('.');
			}
			putchar('\n');
		}
	}
	if (truncate == true)
		printf("                  ........[SNIP].......\n");
}


int init_driller (uint64_t user_seed)
{
	ad_fail = false;

	if (init_buffers() != 0)
		goto fail;

	init_seed(user_seed);
	return 0;

fail:
	ad_fail = true;
	return -1;
}

void exit_driller(void)
{
	destroy_maps();
	destroy_buffers();
}

