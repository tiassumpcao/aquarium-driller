#ifndef _RANDOM_H
#define _RANDOM_H

#ifdef __KERNEL__
#include <linux/random.h>
#else
#include <stdlib.h>
#endif

#include <aquarium-driller/types.h>

/* random */
extern unsigned int seed;
unsigned int		init_seed(uint64_t user_seed);
void				set_seed (void);
void				reseed(void);
unsigned int		new_seed(void);

uint64_t			__get_random_bytes(int nbytes);
unsigned int		rand_bool(void);
unsigned int		rand32(void);
uint64_t			rand64(void);
unsigned int		rand_single_bit(unsigned char);
unsigned long		randbits(int limit);
unsigned long		t4v1s0(void);
unsigned long		rand8x8(void);
unsigned long		rept8(unsigned int num);
unsigned int		__rand32(void);

unsigned long long	get_random_bytes(void *buf, int nbytes);

#define AD_RESEED	reseed()
#define RAND(x)		({AD_RESEED; __get_random_bytes(x);})


#endif  /* _RANDOM_H */

