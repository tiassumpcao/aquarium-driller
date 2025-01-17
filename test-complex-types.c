#include <stdio.h>
#include <stdlib.h>

#include <aquarium-driller/system.h>
#include <aquarium-driller/random.h>
#include <aquarium-driller/variable-generation.h>

#include <inttypes.h>

int main (int argc, char ** argv)
{
	int	argnum = 0;
	int	iterations = 0;
	int index = 0;

	if (argc != 3) {
		printf ("%s <complex arg num> <iteration times>\n", argv[0]);
		exit (1);
	}

	argnum = atoi (argv[1]);
	iterations = atoi (argv[2]);

	AD_INIT_DRILLER(0xdeadbeefbabebeef);

	for (index = 0; index < iterations; index++) {
		printf ("Complex Type %s | Value = %"PRIx64"\n", complex_arg_names[argnum],
			AD_COMPLEX_TYPE(argnum));
		reseed ();
	}

	AD_EXIT_DRILLER;

	return 0;
}


