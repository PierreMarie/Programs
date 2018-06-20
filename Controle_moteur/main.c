#include <stdio.h>
#include <stdlib.h>

#include "wiringPi.h"

#define MEAN 10
#define CONSIGNE 40.0
#define COMMANDE_MIN 8.0
#define COMMANDE_MAX 250.0
#define COMMANDE_INC 0.0001
#define IT_DISPLAY 10000
#define GAIN 0.00002

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
	temp = 0;
	
	pwmWrite (1, 30) ;
	
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
	
		///********************************************************///	REGULATION
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
		
		//commande = (float) GAIN * abs( CONSIGNE - speed_real ) ;
		
		if( speed_real < CONSIGNE )
		{
			commande += (float) GAIN * abs( CONSIGNE - speed_real );
		}
		else
		{
			commande -= (float) GAIN * abs( CONSIGNE - speed_real );
		}
		
		if( commande < COMMANDE_MIN )
		{
			commande = COMMANDE_MIN;
		}
		
		if( commande > COMMANDE_MAX )
		{
			commande = COMMANDE_MAX;
		}
		
		pwmWrite (1, commande) ;
		//pwmWrite (1, 20) ;
		
		cpt++;
		
		///********************************************************///	DISPLAY
		if ( cpt==IT_DISPLAY )
		{
			cpt=0;
			
			if( temp != 0.0 )
			{
				speed_real = (float)100000.0/(1.0*temp);
			}
			else
			{
				speed_real = 0;
			}
			
			printf("Vitesse : %.1f tr/s\t%f\n", speed_real, commande);
		}

		delayMicroseconds(10);
		
		prev = a;
		
	}while(1);
	
	//fclose(fichier);

	return 0 ;
}