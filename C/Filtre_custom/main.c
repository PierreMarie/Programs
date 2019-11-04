#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIZE_STR 10

int main (void)
{
	float input_str[SIZE_STR]={0.0}, sorted_array[2][SIZE_STR]={0.0}, tmp, tmp2;
	int i, j, index=0;
	
	srand(time(NULL));
	
	do
	{
		system("clear");								// Clear console 
		
		for( i=SIZE_STR-1; i>0; i--	)					// Register shifting
		{
			input_str[i] = input_str[i-1];
		}
		
		input_str[i] = 0;
		
		
		tmp = 1 + (rand()%10)/10.0;						// Incoming random value in register
		input_str[0] = tmp;
		
		printf("\n\nAprès insertion:\n");
		
		for(i=0; i<SIZE_STR; i++)						// Register display
		{
			printf("%.2f\t", input_str[i]);
		}
		
		printf("\n");
		
		for(j=0; j<SIZE_STR; j++)						// Counting of each value in register
		{
			if( sorted_array[0][j] == input_str[0] )
			{
				sorted_array[1][j]++;
				break;
			}
			else
			{
				if( j==SIZE_STR-1 )
				{
					sorted_array[0][index] = input_str[0];
					sorted_array[1][index]++;
					index++;
					break;
				}
			}
		}
		
		for( i=0; i<index; i++ )						// Ordering of register according occurence frequency
		{
			for(j=0; j<index-1; j++)
			{
				if( sorted_array[1][j] > sorted_array[1][j+1] )
				{
					tmp = sorted_array[1][j];
					tmp2 = sorted_array[0][j];
					
					sorted_array[1][j] = sorted_array[1][j+1];
					sorted_array[0][j] = sorted_array[0][j+1];
					
					sorted_array[1][j+1] = tmp;
					sorted_array[0][j+1] = tmp2;
				}
			}
		}
		
		printf("\n");
		
		for( i=0; i<SIZE_STR; i++ )						// Sorted register display
		{
			for(j=0; j<2; j++)
			{
				printf("%.2f\t", sorted_array[j][i]);
			}
			
			printf("\n");
			
			if( sorted_array[0][i+1] == 0 )
			{
				break;
			}		
		}
		
		printf("\nMain value : %.2f ", sorted_array[0][index-1]);
			
		sleep(1);
			
	}while(1);

	return 0 ;
}