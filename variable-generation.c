#ifndef __KERNEL__
#include <sys/stat.h>
#else
#include <linux/stat.h>
#endif
#if linux
#include <linux/uio.h>
#else
#include <sys/uio.h>
#endif
#include <aquarium-driller/random.h>
#include <aquarium-driller/system.h>
#include <aquarium-driller/variable-generation.h>

const char *simple_arg_names[] = {
	"SIMPL_TYPE_UNDEFINED",
	"SIMPL_TYPE_BYTE",
	"SIMPL_TYPE_WORD",
	"SIMPL_TYPE_DWORD",
	"SIMPL_TYPE_QWORD",
	NULL
};

const char *complex_arg_names[] = {
	"COMPL_TYPE_UNDEFINED",
	"COMPL_TYPE_RANDOM_LONG",
	"COMPL_TYPE_LEN",
	"COMPL_TYPE_ADDRESS",
	"COMPL_TYPE_MODE_T",
	"COMPL_TYPE_NON_NULL_ADDRESS",
	"COMPL_TYPE_RANGE",
	"COMPL_TYPE_RANDPAGE",
	"COMPL_TYPE_CPU",
	"COMPL_TYPE_IOVEC",
	"COMPL_TYPE_SOCKADDR",
	"COMPL_TYPE_MMAP",
	NULL
};

static uint64_t get_rand_cpu (void)
{
	int i;
	i = RAND(sizeof(int)) % 100;

	switch (i) {
	case 0: return -1;
	case 1: return RAND(sizeof(uint64_t)) % 4095;
	case 2: return RAND(sizeof(uint64_t)) % 15;

	case 3 ... 99:
		return RAND(sizeof(uint64_t)) % NUM_ONLINE_CPUS;

	default:
		break;
	}
	return 0;
}

static uint64_t handle_arg_address (int argnum)
{
	uint64_t addr = 0;

	if (rand_bool())
		return (uint64_t) get_address();

	/* XXX: come up with a clever solution to do incremental address fuzzing
	 * i.e.: addr = find_previous_arg_address(argnum, call, childno);
	 *       ....
	 */
	switch (RAND(sizeof(int)) % 4) {
		case 0:	break;	/* return unmodified */
		case 1:	addr++;
			break;
		case 2:	addr+= sizeof(int);
			break;
		case 3:	addr+= sizeof(long);
			break;
		default:
			break;
	}

	return addr;
}

static uint64_t handle_arg_range(unsigned int argnum)
{
	uint64_t i;
	uint32_t low = 0, high = 0;

	switch (argnum) {
	default:
		break;
	}

	if (high == 0) {
		return 0;
	}

	i = (uint64_t) rand64() % high;
	if (i < low) {
		i += low;
		i &= high;
	}
	return i;
}

static uint64_t handle_arg_randpage(void)
{
		if (rand_bool())
			return (uint64_t) page_allocs;
		else
			return (uint64_t) page_rand;
}

static struct iovec *handle_arg_iovec(unsigned long argnum)
{
	uint64_t i;

	i = (RAND(sizeof(uint64_t)) % 5) + 1;

	switch (argnum) {
	default:
		break;
	}
	return alloc_iovec(i);
}

/* XXX: implement sockaddr fuzzer */
#if 0
static unsigned long handle_arg_sockaddr(unsigned long argnum)
{
	unsigned long sockaddr = 0, sockaddrlen = 0;

	generate_sockaddr(...);

	switch (argnum) {
	default:
		break;
	}

	return (unsigned long) sockaddr;
}
#endif

static mode_t handle_arg_mode_t(void)
{
	unsigned int i, j, count, bit;
	mode_t mode = 0;

	count = RAND(sizeof(int)) % 9;

	for (i = 0; i < count; i++) {
		bit = RAND(sizeof(int)) % 3;
		mode |= 1 << bit;
		j = RAND(sizeof(int)) % 12;
		switch (j) {
		case 0: mode |= S_IRUSR; break;
		case 1: mode |= S_IWUSR; break;
		case 2: mode |= S_IXUSR; break;
		case 3: mode |= S_IRGRP; break;
		case 4: mode |= S_IWGRP; break;
		case 5: mode |= S_IXGRP; break;
		case 6: mode |= S_IROTH; break;
		case 7: mode |= S_IWOTH; break;
		case 8: mode |= S_IXOTH; break;
		case 9: mode |= S_ISUID; break;
		case 10: mode|= S_ISGID; break;
		case 11: mode|= S_ISVTX; break;
		default: break;
		}
	}
	return mode;
}


uint64_t fill_complex_arg (int argnum)
{
	switch (argnum) {
	case COMPL_TYPE_UNDEFINED:
	case COMPL_TYPE_RANDOM_LONG:
		return rand64();

	case COMPL_TYPE_LEN:
		return (uint64_t) get_len();

	case COMPL_TYPE_ADDRESS:
		return (uint64_t) handle_arg_address(argnum);

	case COMPL_TYPE_NON_NULL_ADDRESS:
		return (uint64_t) get_non_null_address();

	case COMPL_TYPE_RANGE:
		return handle_arg_range(argnum);

	case COMPL_TYPE_RANDPAGE:
		return handle_arg_randpage();

	case COMPL_TYPE_CPU:
		return get_rand_cpu();

	case COMPL_TYPE_IOVEC:
		return (uint64_t) handle_arg_iovec(argnum);

/* XXX: implement sockaddr fuzzer
 * case COMPL_TYPE_SOCKADDR:
 *		return handle_arg_sockaddr(childno, call, argnum);
 */

	case COMPL_TYPE_MODE_T:
		return (uint64_t) handle_arg_mode_t();

	default:
		return 0;
	}

	return -1;
}

uint64_t fill_simple_arg (int argnum)
{
	switch (argnum) {
	case SIMPL_TYPE_BYTE:
		return (uint64_t) rand_single_bit(8);
	case SIMPL_TYPE_WORD:
		return (uint64_t) randbits(16);
	case SIMPL_TYPE_DWORD:
		return (uint64_t) rand32();
	case SIMPL_TYPE_QWORD:
		return (uint64_t) rand64();
	default:
		return (uint64_t) rand_bool();
	}
}

/* XXX: we might want to pre-generate vars and initialize
 *      them in a table-like object.
 */
void variable_generation(void)
{
}

