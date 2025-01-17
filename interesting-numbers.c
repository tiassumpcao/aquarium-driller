#include <aquarium-driller/random.h>
#include <aquarium-driller/system.h>

uint32_t get_interesting_32bit_value(void)
{
	switch (RAND(sizeof(int)) % 11) {

		/* common case / small values */
	case 0 ... 7:
		switch (RAND(sizeof(int)) % 8) {
		case 0:
			return 0x00000000;
		case 1:
			return 0x00000001;
		case 2:
			return RAND(sizeof(int)) % 256;
		case 3:
			return 0x00000fff;	// 4095
		case 4:
			return 0x00001000;	// 4096
		case 5:
			return 0x00001001;	// 4097
		case 6:
			return 0x00008000;
		case 7:
			return 0x0000ffff;
		default:
			return 0;
		}
		break;

		/* less common case / more wild */
	case 8 ... 10:
		switch (RAND(sizeof(int)) % 14) {
		case 0:
			return 0x00010000;
		case 1:
			return 0x40000000;
		case 2:
			return 0x7fffffff;
		case 3:
			return 0x80000000;
		case 4:
			return 0x80000001;
		case 5:
			return 0x8fffffff;
		case 6:
			return 0xc0000000;
		case 7:
			return 0xf0000000;
		case 8:
			return 0xff000000;
		case 9:
			return 0xffff0000;
		case 10:
			return 0xffffe000;
		case 11:
			return 0xffffff00 | (RAND(sizeof(int)) % 256);
		case 12:
			return 0xffffffff;
		case 13:
			return 0xffffffff - PAGE_SIZE;
		default:
			return 0;
		}
		break;

	default:
		break;
	}

	return 0;
}

/* XXX: ********* IMPORTANT **********
 *
 * We need to make these richer and entail varous scenarios and platforms.
 * Among them (and very importantly):
 * 1) Windows' interesting addresses
 * 2) Virtualization specific addresses (such as VT-D and IOMMU's special regions)
 */
uint64_t per_arch_interesting_addr(uint32_t low)
{
	uint32_t	high = 0;

	high = RAND(sizeof(int)) % 4;

	switch (high) {
		/* end of x86-64 canonical addr */
	case 0:
		return 0x00007fffffffffffUL;
		/* begin of x86-64 non-canonical addr */
	case 1:
		return 0x0000800000000000UL;
		/* begin of x86-64 canonical addr range 2 */
	case 2:
		return 0xffff800000000000UL | (low << 4);
	case 3:
		return VDSO_ADDR | (low & 0x0fffff);
	default:
		break;
	}

	return high | low;
}

uint64_t get_interesting_64bit_value(void)
{
	uint32_t	low = 0;

	low = get_interesting_32bit_value();

	switch (RAND(sizeof(int)) % 17) {
	case 0:
		return 0;
	case 1:
		return low;
	case 2:
		return 0x0000000100000000UL;
	case 3:
		return 0x0000000100000000UL | low;
	case 4:
		return 0x7fffffff00000000UL;
	case 5:
		return 0x8000000000000000UL;
	case 6:
		return 0xffffffff00000000UL;
	case 7:
		return 0x7fffffff00000000UL | low;
	case 8:
		return 0x8000000000000000UL | low;
	case 9:
		return 0xffffffff00000000UL | low;
	case 10:
		return 0xffffffffffffff00UL | (RAND(sizeof(int)) % 256);
	case 11:
		return 0xffffffffffffffffUL - PAGE_SIZE;
	case 12:
		return PAGE_OFFSET | (low << 4);
	case 13:
		return KERNEL_ADDR | (low & 0xffffff);
	case 14:
		return MODULE_ADDR | (low & 0xffffff);
	case 15:
		return per_arch_interesting_addr(low);
	case 16:
		/* Do we want this to be overflowed? low << 32) */
		return (low << 31);
	default:
		break;
	}
	return 0;
}

