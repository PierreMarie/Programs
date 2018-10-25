#include <stdio.h>
#include <stdlib.h>

#define IT 100000000

int main (void)
{
	float a=0.0;
	long int i;
	
	for( i=0; i<IT; i++ )
	{
		a = 2*(1+i);
	
	}

	return 0;

}