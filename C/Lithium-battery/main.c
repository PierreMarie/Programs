#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define a1 2.585e15
#define b1 38.4
#define c1 6.024
#define a2 32.79
#define b2 3.901
#define c2 0.1677
#define a3 72.0
#define b3 4.202
#define c3 0.2653
#define MAX_STR 10

int main (void)
{
	float percentage, value1, value2, value3, voltage;
	char chaine[MAX_STR];
	
	do
	{
		printf("Voltage ? (0.0 - 42.0) : ");
		fgets(chaine, MAX_STR, stdin);
		
		voltage = atof(chaine);
		
		voltage = voltage / 10.0;
		
		value1 = a1 * exp(-1.0 * pow(((voltage-b1)/c1),2));
		value2 = a2 * exp(-1.0 * pow(((voltage-b2)/c2),2));
		value3 = a3 * exp(-1.0 * pow(((voltage-b3)/c3),2));
		
		percentage = value1 + value2 + value3;
		printf("Percentage : %.1f %\n\n", percentage);

	}while(1);
}
