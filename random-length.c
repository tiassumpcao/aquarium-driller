#include <aquarium-driller/random.h>
#include <aquarium-driller/system.h>
#include <aquarium-driller/types.h>

size_t get_len(void)
{
	int i = 0;

	i = rand32();

	if (i == 0)
		return 0;

	switch (RAND(sizeof(int)) % 6) {

	case 0:
		i &= 0xff;
		break;
	case 1:
		i &= PAGE_SIZE;
		break;
	case 2:
		i &= 0xffff;
		break;
	case 3:
		i &= 0xffffff;
		break;
	case 4:
		i &= 0xffffffff;
		break;
	default:
		break;
	}

	if (i == 0)
		return 0;

	if (RAND(sizeof(int)) % 100 < 25) {
		switch (RAND(sizeof(int)) % 3) {
		case 0:
			i /= sizeof(int);
			break;
		case 1:
			i /= sizeof(long);
			break;
		case 2:
			i /= sizeof(long long);
			break;
		default:
			break;
		}
	}

	return i;
}

