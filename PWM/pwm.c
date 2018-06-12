#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"

// void pwmWrite (int pin, int value)		value = 0 - 1024
// void pwmSetRange (unsigned int range)	range ? - 4096
// void pwmSetClock (int divisor)		range ? - 4096		default 1024

// PWM frequency = 19.2 MHz / ( pwmClock * pwmRange )
// Min frequency = 1.14 Hz

// void delayMicrosecondsHard (unsigned int howLong)
// void delayMicroseconds (unsigned int howLong)


int main (void)
{
	int bright;

	if (wiringPiSetup () == -1)
	exit (1) ;

	pinMode (1, PWM_OUTPUT) ;

	for (;;)
	{
	for (bright = 0 ; bright < 1024 ; ++bright)
	{
		pwmWrite (1, bright) ;
		delay (1) ;
	}

	for (bright = 1023 ; bright >= 0 ; --bright)
	{
		pwmWrite (1, bright) ;
		delay (1) ;
	}

	}

	return 0 ;
}
