#include <stdio.h>

#include <aquarium-driller/random.h>

#define INTEGER get_interesting_32bit_value()

void function (unsigned long var)
{
	printf ("%lX\n", var);
}

int main (void)
{
	printf ("ho\n");
	init_seed ();
	printf ("hi\n");
	function (INTEGER);

	return 0;
}


