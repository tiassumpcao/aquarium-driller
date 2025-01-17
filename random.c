#ifndef __KERNEL__
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#endif

#include <aquarium-driller/random.h>
#include <aquarium-driller/variable-generation.h>

unsigned int rand_bool(void)
{
	return RAND(1) % 2;
}

unsigned int rand_single_bit(unsigned char size)
{
	return (1L << (RAND(size) % size));
}

unsigned long randbits(int limit)
{
	unsigned int num = RAND(sizeof(int)) % limit / 2;
	unsigned int i;
	unsigned long r = 0;

	for (i = 0; i < num; i++)
		r |= (1 << (RAND(sizeof(int)) % (limit - 1)));

	return r;
}

unsigned long t4v1s0(void)
{
	unsigned long r = 0;

	switch (RAND(sizeof(int)) % 4) {
	case 0:
		r = RAND(sizeof(int)) & RAND(sizeof(int));
#if __WORDSIZE == 64
		r <<= 32;
		r |= RAND(sizeof(int)) & RAND(sizeof(int));
#endif
		break;

	case 1:
		r = RAND(sizeof(int)) % RAND(sizeof(int));
#if __WORDSIZE == 64
		r <<= 32;
		r |= RAND(sizeof(int)) % RAND(sizeof(int));
#endif
		break;

	case 2:
		r = RAND(sizeof(int)) | RAND(sizeof(int));
#if __WORDSIZE == 64
		r <<= 32;
		r |= RAND(sizeof(int)) | RAND(sizeof(int));
#endif
		break;

	case 3:
		r = RAND(sizeof(int));
#if __WORDSIZE == 64
		r <<= 32;
		r |= RAND(sizeof(int));
#endif
		break;

	default:
		break;
	}

	return r;
}

unsigned long rand8x8(void)
{
	unsigned long r = 0UL;
	unsigned int i;

	for (i = (RAND(sizeof(int)) % 7) + 1; i > 0; --i)
		r = (r << 8) | RAND(sizeof(int)) % 256;

	return r;
}

unsigned long rept8(unsigned int num)
{
	unsigned long r = 0UL;
	unsigned int i;
	unsigned char c;

	c = RAND(sizeof(int)) % 256;
	for (i = RAND(sizeof(int)) % (num - 1); i > 0; --i)
		r = (r << 8) | c;

	return r;
}

unsigned int __rand32(void)
{
	unsigned long r = 0;

	switch (RAND(sizeof(int)) % 7) {
	case 0:
		r = rand_single_bit(32);
		break;
	case 1:
		r = randbits(32);
		break;
	case 2:
		r = RAND(sizeof(int));
		break;
	case 3:
		r = t4v1s0();
		break;
	case 4:
		r = rand8x8();
		break;
	case 5:
		r = rept8(4);
		break;
	case 6:
		return get_interesting_32bit_value();
	default:
		break;
	}

	return r;
}

unsigned int rand32(void)
{
	unsigned long r = 0;

	r = __rand32();

	if (rand_bool()) {
		unsigned int i;
		unsigned int rounds;

		rounds = RAND(sizeof(int)) % 3;
		for (i = 0; i < rounds; i++) {
			switch (rand_bool()) {
			case 0:
				r |= __rand32();
				break;
			case 1:
				r ^= __rand32();
				break;
			default:
				break;
			}
		}
	}

	if (rand_bool())
		r = INT_MAX - r;

	if (rand_bool())
		r |= (1L << 31);

	/* limit the size */
	switch (RAND(sizeof(int)) % 4) {
	case 0:
		r &= 0xff;
		break;
	case 1:
		r &= 0xffff;
		break;
	case 2:
		r &= 0xffffff;
		break;
	default:
		break;
	}

	return r;
}

uint64_t __get_random_bytes(int nbytes)
{
	uint64_t value = 0;

	if (nbytes > sizeof(value))
		return -1;

	get_random_bytes(&value, nbytes);
	return value;
}

uint64_t rand64(void)
{
	unsigned long r = 0;

	if (rand_bool()) {
		/* 32-bit ranges. */
		r = rand32();

	} else {
		/* 33:64-bit ranges. */
		switch (RAND(sizeof(int)) % 7) {
		case 0:
			r = rand_single_bit(64);
			break;
		case 1:
			r = randbits(64);
			break;
		case 2:
			r = rand32() | rand32() << 31;
			break;
		case 3:
			r = t4v1s0();
			break;
		case 4:
			r = rand8x8();
			break;
		case 5:
			r = rept8(8);
			break;
			/* sometimes pick a not-so-random number. */
		case 6:
			return get_interesting_64bit_value ();
		default:
			break;
		}

		/* limit the size */
		switch (RAND(sizeof(int)) % 4) {
		case 0:
			r &= 0x000000ffffffffffULL;
			break;
		case 1:
			r &= 0x0000ffffffffffffULL;
			break;
		case 2:
			r &= 0x00ffffffffffffffULL;
			break;
		default:
			break;
		}

	}

	if (rand_bool())
		r = ~r;

	/* increase distribution in the MSB */
	if ((RAND(sizeof(int)) % 10)) {
		unsigned int i;
		unsigned int rounds;

		rounds = RAND(sizeof(int)) % 4;
		for (i = 0; i < rounds; i++)
			r |= (1L << (__WORDSIZE - (RAND(sizeof(int)) % 256)));
	}

	/* randomly flip sign bit. */
	if (rand_bool())
		r |= (1L << (__WORDSIZE - 1));

	return r;
}

