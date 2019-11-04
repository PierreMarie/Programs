#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

double recursive( long int n)
{
	int result;
	
	if( n <= 1 )
	{
		result = 1.0;
	}
	else
	{
		result = n*recursive(n-1);
	}
	
	return result;	
}

int main(void)
{
	long int input = 14;
	double result = recursive(input);
	
	printf("\nFactorielle de %ld = %f\n\n",input, result);	
	
	return 0;
}