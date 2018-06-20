#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAX_INT 30
#define SIZE_STR 3
#define FLOAT_DIVIDER 3

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

	do
	{
		if( str[i]!=0 )
		{
			i++;
		}
		else
		{
			return i;
		}

	}while( i<(SIZE_STR-1) );

	return SIZE_STR;
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

void calculate_median(float* str, int size)
{
	int middle = size/2;
	
	printf("\nMiddle index : %d\tMedian : %.1f\n\n", middle, str[middle]);
}

void display_str(float* str, int size)
{
	int i;

	printf("\nString: ");

	for(i=0; i<size; i++)
	{
		if( i==(size-1) )
		{
			printf("%.1f", str[i]);
		}
		else
		{
			printf("%.1f, ", str[i]);
		}
	}

	printf("\n\n");
}

int main (void)
{
	float str[SIZE_STR]={0};
	
	srand(time(NULL));
	
	generate_str(str, SIZE_STR);
	display_str(str, SIZE_STR);
	class(str);
	display_str(str, SIZE_STR);
	calculate_median(str, SIZE_STR);

	return 0 ;
}
