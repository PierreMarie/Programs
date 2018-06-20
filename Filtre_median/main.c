#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAX_INT 10
#define SIZE_STR 20
#define FLOAT_DIVIDER 1.1

void generate_str(float* str, int size)
{
	int i, k, test;
	float tmp;

	for(i=0; i<size; i++)
	{
		//do
		//{
		//	test=0;
			//tmp=(rand()%MAX_INT)+1;
			tmp=(float)((rand()%MAX_INT)+1)/FLOAT_DIVIDER;

		/*	for(k=0; k<i; k++)
			{
				if( tmp==str[k] )
				{
					test=1;
				}
			}

		}while(test);*/

		str[i] = tmp;
	}
}

int calculate_size(float* str)
{
	int i=0;

	while( str[i]!=0 )
	{
		i++;
	}
	
	return i;
}

void class(float* str)
{
	int i, j, size=calculate_size(str);
	float tmp;

	for(i=0; i<size; i++)
	{
		for(j=0; j<size; j++)
		{
			if( j<(size-1) )
			{
				if( str[j]>str[j+1] )
				{
					tmp=str[j];
					str[j]=str[j+1];
					str[j+1]=tmp;
				}
			}
		}
	}
}

void calculate_median(float* str)
{
	int middle;

	middle = calculate_size(str) / 2;
	
	printf("\nMiddle index : \t\t%d\tMedian : \t%.1f\n\n", middle, str[middle]);
}

void calculate_mean(float* str)
{
	int i, size = calculate_size(str);
	float cpt = 0.0;
	
	for( i=0; i<size; i++ )
	{
		cpt += str[i];
	}
	
	cpt /= size;
	
	printf("\nNumber of elements : \t%d\tMean : \t\t%.1f\n\n", size, cpt);
}

void display_str(float* str)
{
	int i = 0;

	printf("\nString: ");

	for( i=0; i<calculate_size(str)-1; i++)
	{
		printf("%.1f, ", str[i]);
	}
	
	printf("%.1f\n\n", str[i]);
}

int main (void)
{
	float str[SIZE_STR]={0};
	
	srand(time(NULL));
	
	generate_str(str, SIZE_STR);
	display_str(str);
	class(str);
	display_str(str);
	calculate_median(str);
	calculate_mean(str);

	return 0 ;
}
