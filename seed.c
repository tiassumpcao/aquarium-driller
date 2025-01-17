#ifndef __KERNEL__
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <limits.h>
#else
#include <linux/stat.h>
#endif

#include <aquarium-driller/random.h>

unsigned int seed = 0;
uint64_t prime = 12764787846358441471ULL;

#ifndef __KERNEL__
unsigned long long get_random_bytes(void *buf, int nbytes)
{
	size_t lo, hi;
	struct timeval tv;
	unsigned long long value = 0;

	asm volatile (
		"rdtsc"
		: "=a"(lo), "=d" (hi)
		: "a"(0)
		: "%ebx", "%ecx");

	gettimeofday(&tv, NULL);
	value = (unsigned long long) (tv.tv_usec | (((hi & 0x000ff) | (lo << 8)) << 20));
	value = value * prime * rand();
	memcpy (buf, &value, nbytes);

	return value;
}
#endif

unsigned int new_seed(void)
{
	unsigned int r;

	get_random_bytes(&r, sizeof(unsigned int));
	return r;
}

unsigned int init_seed(uint64_t user_seed)
{
	if (user_seed != 0)
		seed = user_seed;
	else
		seed = new_seed();

	return seed;
}

void set_seed(void)
{
	reseed ();
}

void reseed(void)
{
	seed = new_seed();
}

