#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

///	CYCLE TOTAL = 30 µs

#define MEAN 10
#define CONSIGNE 30.0
#define COMMANDE_MIN 0.0
#define COMMANDE_MAX 200.0
#define COMMANDE_INC 0.0001
#define IT_DISPLAY 1
#define MAX_COUNT 100000.0			// 1 tr/s
#define LOOP_DURATION 10
#define GAIN_TEMPO 1.0E6 / LOOP_DURATION

#define Te 0.00001
#define Kp 0.5					//#define GAIN 0.00002
#define Ti 10

// 	Correcteur: u(k) = u(k-1) + Kp*( Te/Ti + 1 )*e(k) - Kp*e(k-1) 
//	J = 0,002.10-6 Kg.m2

pthread_mutex_t mutex_tab_speed = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);

float tab_speed[MEAN]={0.0}, speed_real;

int main (void)
{
	char test_dead_lock, prev;
	long int a, b, c, tab_mean[MEAN]={0}, tab_median[MEAN]={0}, tmp, cpt;
	int i, j, middle;
	long int sum;
	float temp, commande;
	
	pthread_t thread1;
	
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
	
	commande = CONSIGNE;
	pwmWrite (1, commande) ;
	speed_real = 0.0;
	temp = 0.0;
	
	/*if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }*/
	
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
		
			///********************************************************///	UPDATE REAL SPEED
			//pthread_mutex_lock(&mutex_speed);
			
			if( (temp == 0.0) || (temp > MAX_COUNT) )	//|| (test_dead_lock == 1) )
			{
				speed_real = 0.0;
			}
			else
			{
				speed_real = (float)GAIN_TEMPO/(1.0*temp);
			}
			
			//pthread_mutex_unlock(&mutex_speed);
			
			///********************************************************///	UPDATE TAB_SPEED
			//pthread_mutex_lock(&mutex_tab_speed);
			
			for( i=0; i<MEAN-1; i++ )
			{
				tab_speed[i] = tab_speed[i+1];
			}
			
			//pthread_mutex_lock(&mutex_speed);
			
			tab_speed[MEAN-1] = speed_real;
			
			//pthread_mutex_unlock(&mutex_speed);
			
			//pthread_mutex_unlock(&mutex_tab_speed);
			
			///********************************************************///	REGULATION
			//pthread_mutex_lock(&mutex_speed);
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
			
			//pthread_mutex_unlock(&mutex_speed);
			
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
			
			
			///********************************************************///	DISPLAY
			//pthread_mutex_lock(&mutex_speed);
			cpt++;
			
			if(cpt > IT_DISPLAY)
			{
				printf("Speed : %.1f\t\tCmd : %.1f\t\tRef : %.1f\n", speed_real, commande, CONSIGNE);
				cpt = 0;
			}
			//pthread_mutex_unlock(&mutex_speed);
		}
		
		if( a==1 ) 
		{
			b++;
		}
		else
		{
			c++;
		}	
		
		prev = a;
		
		delayMicroseconds(LOOP_DURATION);
		
	}while(1);
	
	//fclose(fichier);

	return 0 ;
}

void *thread_1(void *arg)
{
	float val_dead_lock;
	char test_dead_lock;
	int i;
	
	printf("Nous sommes dans le thread1\n");
	
	//delay(500);
	
	///********************************************************///	TEST DEAD LOCK
	
	do
	{
		pthread_mutex_lock(&mutex_speed);
		
		if( speed_real > 1.0 )
		{
			test_dead_lock = 1;
		}
		
		pthread_mutex_unlock(&mutex_speed);
		
		pthread_mutex_lock(&mutex_tab_speed);
		
		//val_dead_lock = tab_speed[0];
		
		//for( i=1; i<MEAN; i++ )
		{
			if( abs(tab_speed[0]-tab_speed[MEAN-1]) > 0.000001 ) test_dead_lock = 0;
		}
		
		printf("\n%.10f\t%.10f\n", tab_speed[0], tab_speed[MEAN-1]);
		
		pthread_mutex_unlock(&mutex_tab_speed);
		
		pthread_mutex_lock(&mutex_speed);
		
		if( test_dead_lock==1 )
		{
			printf("LOCK !!\n");
			speed_real = 0.0;
		}
		
		pthread_mutex_unlock(&mutex_speed);
		
		//delayMicroseconds(LOOP_DURATION*10);
		delay(500);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}