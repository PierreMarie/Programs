#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

int main (void)
{
	if (wiringPiSetup () == -1)
	exit (1) ;
	pinMode (25, OUTPUT) ;
	
	do
	{
		digitalWrite(25, HIGH);
		digitalWrite(25, LOW);
	
		//delayMicroseconds(10);
		
	}while(1);
	
	return 0 ;
}