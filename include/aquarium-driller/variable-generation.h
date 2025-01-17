#ifndef	_VARIABLE_GENERATION_H
#define _VARIABLE_GENERATION_H

#include <aquarium-driller/types.h>

uint64_t	fill_simple_arg (int argnum);
uint64_t	fill_complex_arg (int argnum);
void		variable_generation (void);
bool		validate_address(void *addr);

/* random-length */
unsigned long	get_len(void);

/* random-page */
void			generate_random_page(char *);

/* interesting-numbers */
uint32_t		get_interesting_32bit_value(void);
uint64_t		per_arch_interesting_addr(uint32_t);
uint64_t		get_interesting_64bit_value(void);

/* random-address */
void			*get_address(void);
struct			iovec *alloc_iovec(unsigned int num);
void			*get_non_null_address(void);


#define	AD_GENERATE_VARIABLE(x) 			(x)
#define AD_INTERESTING_32BIT_VALUE			get_interesting_32bit_value()
#define AD_INTERESTING_64BIT_VALUE			get_interesting_64bit_value()
#define AD_PER_ARCH_INTERESTING_ADDR(x)		per_arch_interesting_addr(x)
#define AD_COMPLEX_TYPE(x)					fill_complex_arg(x)
#define AD_SIMPLE_TYPE(x)					fill_simple_arg(x)

extern const char *simple_arg_names[];
extern const char *complex_arg_names[];

enum simple_argtype {
	SIMPL_TYPE_UNDEFINED = 1 << 0,
	SIMPL_TYPE_BYTE = 1 << 1,
	SIMPL_TYPE_WORD = 1 << 2,
	SIMPL_TYPE_DWORD = 1 << 3,
	SIMPL_TYPE_QWORD = 1 << 4,
	SIMPL_TYPE = 4
};

enum complex_argtype {
	COMPL_TYPE_UNDEFINED = 0,
	COMPL_TYPE_RANDOM_LONG = 1,
	COMPL_TYPE_LEN = 2,
	COMPL_TYPE_ADDRESS = 3,
	COMPL_TYPE_MODE_T = 4,
	COMPL_TYPE_NON_NULL_ADDRESS = 5,
	COMPL_TYPE_RANGE = 6,
	COMPL_TYPE_RANDPAGE = 7,
	COMPL_TYPE_CPU = 8,
	COMPL_TYPE_IOVEC = 9,
	COMPL_TYPE_SOCKADDR = 10,
	COMPL_TYPE_MMAP = 11,
	COMPL_TYPE = COMPL_TYPE_MMAP
};

#endif /* _VARIABLE_GENERATION_H */

