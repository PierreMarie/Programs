#include <stdio.h>
#include <stdlib.h>

#include "wiringPi.h"

///	CYCLE TOTAL = 30 µs

#define MEAN 10
#define CONSIGNE 20.0
#define COMMANDE_MIN 0.0
#define COMMANDE_MAX 50.0
#define COMMANDE_INC 0.0001
#define IT_DISPLAY 10000
#define MAX_COUNT 100000.0			// 1 tr/s
#define LOOP_DURATION 10
#define GAIN_TEMPO 1.0E6 / LOOP_DURATION

#define Te 0.00001
#define Kp 0.001					//#define GAIN 0.00002
#define Ti 10

// 	Correcteur: u(k) = u(k-1) + Kp*( Te/Ti + 1 )*e(k) - Kp*e(k-1) 
//	J = 0,002.10-6 Kg.m2

int main (void)
{
	long int a, b, c, cpt;
	int tab_mean[MEAN]={0}, tab_median[MEAN]={0}, i, j, middle, tmp, prev;
	long int sum;
	float temp, speed_real, commande;
	
	//FILE* fichier = NULL;
	//fichier = fopen("file.txt", "a");
	
	if (wiringPiSetup () == -1)
	exit (1) ;
	pinMode (24, INPUT) ;
	pinMode (1, PWM_OUTPUT) ;
	//pullUpDnControl (25, PUD_UP);
	
	b = 0;		// 1
	c = 0;		// 0
	prev = 1;
	cpt=0;
	
	commande = CONSIGNE;
	pwmWrite (1, commande) ;
	speed_real = 0;
	temp = 0.0;
	
	do
	{
		a = digitalRead(24);
		//fprintf(fichier, "%d\n", a);
		
		if ( prev==0 && a==1 )
		{		
			///********************************************************///	MEAN
			sum = 0;
		
			for( i=0; i<MEAN-1; i++ )
			{
				tab_mean[i] = tab_mean[i+1];
				sum += tab_mean[i];	
			}
			
			tab_mean[MEAN-1] = b+c;
			
			//sum += tab_mean[MEAN-1];
			//temp=(float)sum/MEAN;
			
			///********************************************************///	MEDIAN
			for( i=0; i<MEAN; i++ )
			{
				tab_median[i] = tab_mean[i];
			}
			
			middle=MEAN/2;

			for(i=0; i<MEAN; i++)
			{
				for(j=0; j<MEAN; j++)
				{
					if( j<(MEAN-1) )
					{
						if( tab_median[j]>tab_median[j+1] )
						{
							tmp=tab_median[j];
							tab_median[j]=tab_median[j+1];
							tab_median[j+1]=tmp;
						}
					}
				}
			}
			
			temp = (float)tab_median[middle];
			
			b = 0;
			c = 0;
		}
		
		if( a==1 ) 
		{
			b++;
		}
		else
		{
			c++;
		}	
		
		///********************************************************///	UPDATE REAL SPEED
		if( (temp == 0.0) || (temp > MAX_COUNT) )
		{
			speed_real = 0.0;
		}
		else
		{
			speed_real = (float)GAIN_TEMPO/(1.0*temp);
		}
		
		///********************************************************///	REGULATION
		///*************************************///	Fourchette
		/*if( speed_real < CONSIGNE )
		{
			if( commande < COMMANDE_MAX )
			{
				commande += COMMANDE_INC;
			}
		}
		else
		{
			if( commande > CONSIGNE )
			{
				if( commande > COMMANDE_MIN )
				{
					commande -= COMMANDE_INC;
				}
			}
		}*/
		
		///*************************************///	P
		commande += Kp * ( CONSIGNE - speed_real );

		///*************************************///	PI
		//commande +=  Kp * ( CONSIGNE - speed_real ) + ( Kp * Te / Ti * ( CONSIGNE - speed_real ) );

		
		///*************************************///	Ecretage
		if( commande < COMMANDE_MIN )
		{
			commande = COMMANDE_MIN;
		}
		
		if( commande > COMMANDE_MAX )
		{
			commande = COMMANDE_MAX;
		}
		
		pwmWrite (1, commande) ;
		
		cpt++;
		
		///********************************************************///	DISPLAY
		if ( cpt==IT_DISPLAY )
		{
			cpt=0;
			
			printf("Vitesse : %.1f tr/s\t%f\t%f\n", speed_real, commande, temp);
		}

		delayMicroseconds(LOOP_DURATION);
		
		prev = a;
		
	}while(1);
	
	//fclose(fichier);

	return 0 ;
}