#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#endif

#if linux
#include <linux/uio.h>
#else
#include <sys/uio.h>
#endif

#include <aquarium-driller/random.h>
#include <aquarium-driller/system.h>
#include <aquarium-driller/types.h>
#include <aquarium-driller/memory-maps.h>

static bool within_page(void *addr, void *check)
{
	if (addr == check)
		return true;
	if ((addr > check) && (addr < (check + PAGE_SIZE)))
		return true;
	return false;
}

bool validate_address(void *addr)
{
	if (within_page(addr, page_rand) == true)
		return false;
	if (within_page(addr, page_zeros) == true)
		return false;
	if (within_page(addr, page_0xff) == true)
		return false;
	if (within_page(addr, page_allocs) == true)
		return false;

	return true;
}

static void *_get_address(unsigned char null_allowed)
{
	int i;
	void *addr = NULL;
	struct map *map = NULL;

	if (null_allowed == true)
		i = RAND(sizeof(int)) % 9;
	else
		i = (RAND(sizeof(int)) % 8) + 1;

	switch (i) {
	case 0:
		addr = NULL;
		break;
	case 1:
		addr = (void *) KERNEL_ADDR;
		break;
	case 2:
		addr = page_zeros;
		break;
	case 3:
		addr = page_0xff;
		break;
	case 4:
		addr = page_rand;
		break;
	case 5:
		addr = page_allocs;
		break;
	case 6:
		addr = (void *) rand64();
		break;
	case 7:
		map = get_random_map();
		if (map == NULL)
			break;
		addr = map->ptr;
		break;
	case 8:
		addr = ALLOC_MAX4M_REGULAR(PAGE_SIZE * 2);
		break;
	default:
		break;
	}

	i = RAND(sizeof(int)) % 100;

	switch (i) {
	case 0:
		addr += (PAGE_SIZE - sizeof(char));
		break;
	case 1:
		addr += (PAGE_SIZE - sizeof(int));
		break;
	case 2:
		addr += (PAGE_SIZE - sizeof(long));
		break;
	case 3:
		addr += (PAGE_SIZE / 2);
		break;
	case 4 ... 99:
	default:
		break;
	}

	return addr;
}

void *get_address(void)
{
	return _get_address(true);
}

void *get_non_null_address(void)
{
	return _get_address(false);
}

void * get_pc (void)
{
	return __builtin_return_address(0);
}

/*
 * iovec's are just special cases of the ARG_ADDRESS's
 */
struct iovec *alloc_iovec(unsigned int num)
{
	struct iovec *iov;
	unsigned int i;

	iov = ALLOC_MAX4M_REGULAR(num * sizeof(struct iovec));

	if (iov != NULL) {
		for (i = 0; i < num; i++) {
			iov[i].iov_base = ALLOC_MAX4M_REGULAR(PAGE_SIZE);
			iov[i].iov_len = PAGE_SIZE;
		}
	}
	return iov;
}

