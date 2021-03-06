#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"

// void pwmSetMode (int mode)			PWM_MODE_BAL, PWM_MODE_MS 	default balanced (foireux)

// void pwmWrite (int pin, int value)		range 0 - 1024 	?
// void pwmSetRange (unsigned int range)	range ? - 4096	?	This sets the resolution, the number of steps between 0 and 100% duty cycle. The default value is 1024.
// void pwmSetClock (int divisor)		range 2 - 4095	?	default 1024 or 32 ?

// PWM frequency = 19.2 MHz / ( pwmClock * pwmRange )
// Min frequency = 1.14 Hz

// void delayMicrosecondsHard (unsigned int howLong)
// void delayMicroseconds (unsigned int howLong)

// void pullUpDnControl (int pin, int pud)	PUD_UP, PUD_DOWN, PUD_OFF

// void pinMode (int pin, int mode)		INPUT, OUTPUT, PWM_OUTPUT, GPIO_CLOCK, SOFT_PWM_OUTPUT, SOFT_TONE_OUTPUT, PWM_TONE_OUTPUT
// void digitalWrite (int pin, int value)	LOW, HIGH

#define MAX 760		// 4.40V
#define MIN 375		// 2.69V
#define DELAY 15

int main (void)
{
	int bright;

	if (wiringPiSetup () == -1)
	exit (1) ;

	pinMode (1, PWM_OUTPUT) ;
	
	//pwmWrite (1, MIN) ;
	//pinMode (24, OUTPUT) ;
	
	//digitalWrite (24, HIGH);

	for (;;)
	{
	for (bright = MIN ; bright < MAX ; ++bright)
	{
		pwmWrite (1, bright) ;
		delay (DELAY) ;
	}

	for (bright = MAX ; bright >= MIN ; --bright)
	{
		pwmWrite (1, bright) ;
		delay (DELAY) ;
	}

	}

	return 0 ;
}
