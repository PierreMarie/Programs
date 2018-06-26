#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

///	CYCLE TOTAL = 30 µs

#define MEAN 5
#define CONSIGNE 30.0
#define COMMANDE_MIN 0.0
#define COMMANDE_MAX 200.0
#define COMMANDE_INC 0.1
#define MAX_COUNT 100000.0			// 1 tr/s
#define LOOP_DURATION 10
#define GAIN_TEMPO 1.0E6 / LOOP_DURATION
#define PERIODE_ECH 100
#define UNBLOCKING 100

#define Te 0.00001
#define KP 0.1					//#define GAIN 0.00002 0.4
#define Ti 0.01

// 	Correcteur: u(k) = u(k-1) + Kp*( Te/Ti + 1 )*e(k) - Kp*e(k-1) 
//	J = 0,002.10-6 Kg.m2

pthread_mutex_t mutex_update = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed_real = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);

float speed_real, Kp=KP, erreur, commande;
long int b, c;
char update;

int main (void)
{
	char prev;
	long int a, tab_mean[MEAN]={0}, tab_median[MEAN]={0}, tmp;
	int i, j, middle;
	long int sum;
	float temp;
	
	pthread_t thread1;
	
	/*FILE* fichier = NULL;
	fichier = fopen("file.txt", "w+");
	fprintf(fichier, "%f\n", speed_real);*/
	
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
	
	update = 0;
	
	if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	do
	{
		a = digitalRead(24);
		
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
			for( i=0; i<MEAN; i++ ) tab_median[i] = tab_mean[i];
			
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
			
			pthread_mutex_lock(&mutex_speed_real);
			
			if( temp == 0.0 ) speed_real = 0.0;
			else speed_real = (float)GAIN_TEMPO/(1.0*temp);
		
			pthread_mutex_unlock(&mutex_speed_real);
			
			pthread_mutex_lock(&mutex_update);
			update = 1;
			pthread_mutex_unlock(&mutex_update);
			
			
			///********************************************************///	REGULATION
			erreur = CONSIGNE - speed_real;
			///*************************************///	Fourchette
			/*if( speed_real < CONSIGNE )
			{
				if( commande < COMMANDE_MAX ) commande += COMMANDE_INC;
			}
			else
			{
				if( commande > COMMANDE_MIN ) commande -= COMMANDE_INC;
			}*/
			
			///*************************************///	P			
			commande += Kp * erreur;
	
			///*************************************///	PI
			//commande +=  Kp * ( CONSIGNE - speed_real ) + ( Kp * Te / Ti * ( CONSIGNE - speed_real ) );
			
			///*************************************///	Ecretage
			if( commande < COMMANDE_MIN ) commande = COMMANDE_MIN;
			if( commande > COMMANDE_MAX ) commande = COMMANDE_MAX;
			
			pwmWrite (1, commande) ;
			//pwmWrite (1, 40) ;
		}
		
		if( a==1 ) 	b++;
		else 		c++;	
		
		prev = a;
		delayMicroseconds(LOOP_DURATION);
		
	}while(1);
	
	//fclose(fichier);

	return 0 ;
}

void *thread_1(void *arg)
{
	printf("En attente du démarrage du moteur ...\n");
	
	///********************************************************///	TEST DEAD LOCK
	
	do
	{
		if( b+c > MAX_COUNT )
		{
			pthread_mutex_lock(&mutex_speed_real);
			speed_real = 0;
			pthread_mutex_unlock(&mutex_speed_real);
			
			pwmWrite (1, UNBLOCKING) ;
		}
		
	///********************************************************///	DISPLAY
		if( update == 1 )
		{
			pthread_mutex_lock(&mutex_update);
			update = 0;
			pthread_mutex_unlock(&mutex_update);
			
			printf("Speed : %.1f\t\tCmd : %.1f\t\tRef : %.1f\n", speed_real, commande, CONSIGNE);
		}
		
		delay(PERIODE_ECH);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}