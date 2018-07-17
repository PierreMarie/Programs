#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

///	CYCLE TOTAL = 30 µs

#define MEAN 5
#define CONSIGNE 10.0
#define COMMANDE_MIN 0.0
#define COMMANDE_MAX 80.0
#define COMMANDE_INC 0.1
#define MAX_COUNT 50000.0			// 1 tr/s
#define LOOP_DURATION 10
#define GAIN_TEMPO 1.0E6 / LOOP_DURATION
#define GAIN_TEMPO_LOW 600.0
#define PERIODE_ECH 100
#define UNBLOCKING 100

#define Te 0.01
//#define Kp 0.6					// Tosc = 500ms pour Kp = 1.2
//#define Ti 0.01

#define ADD 2.0

#define Kosc 1.2
#define Tosc 0.6

#define KPp 0.5*Kosc

#define KPIi (0.54*Kosc/Tosc) 			// 1.3
#define KPIp (0.45*Kosc - 0.5*KPIi*Te )	//0.47

#define KPIDi 1.2*Kosc/Tosc
#define KPIDp 0.6*Kosc - 0.5*KPIDi*Te
#define KPIDd 3.0/40.0*Kosc*Tosc

//#define b0 -KPIp
//#define b1 KPIp+KPIi

//#define b0 -0.5					//	-Kp
//#define b1 1.0					//	(Kp + Kp*Te/Ti)

// 	Correcteur: u(k) = u(k-1) + Kp*( Te/Ti + 1 )*e(k) - Kp*e(k-1) 
//	J = 0,002.10-6 Kg.m2

//commande = commande_prev + b1*erreur + b0*erreur_prev;

pthread_mutex_t mutex_update = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed_real = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);
void *thread_2(void *arg);
//void *thread_3(void *arg);

float speed_real, speed_real_low, erreur, erreur_prev, erreur_prev_prev, commande, commande_prev;
long int b, c;
char update, work;
FILE* fichier = NULL;

int main (void)
{
	int i;
	
	pthread_t thread1;
	pthread_t thread2;
	//pthread_t thread3;
	
	fichier = fopen("file.txt", "w+");
	
	if (wiringPiSetup () == -1)
	exit (1) ;
	pinMode (24, INPUT) ;
	
	pinMode (23, INPUT) ;
	//pullUpDnControl (23, PUD_UP);
	
	pinMode (1, PWM_OUTPUT) ;
	
	b = 0;		// 1
	c = 0;		// 0
	
	commande = CONSIGNE;
	commande_prev = CONSIGNE;
	
	erreur = 0.0;
	erreur_prev = 0.0;
	erreur_prev_prev = 0.0;
	
	speed_real = 0.0;
	speed_real_low = 0.0;
	
	pwmWrite (1, commande);
	/*pwmWrite (1, 50);
	
	for(i=30; i<80; i++)
	{
		pwmWrite (1, i);
		printf("%d\n", i);
		delay(1000);
	}*/
	
	if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	if(pthread_create(&thread2, NULL, thread_2, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	/*if(pthread_create(&thread3, NULL, thread_3, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;*/
	
	while(1);

	return 0;
}

void *thread_1(void *arg)
{
	printf("En attente du démarrage du moteur ...\n");
	printf("\nKp : %f\t\tKi : %f\t\tKd : %f\n\n", KPIDp, KPIDi, KPIDd);
	
	///********************************************************///	TEST DEAD LOCK
	do
	{
		if( b+c > MAX_COUNT )
		{
			pthread_mutex_lock(&mutex_speed_real);
			speed_real = 0;
			speed_real_low = 0;
			pthread_mutex_unlock(&mutex_speed_real);
		}
		
		///********************************************************///	DISPLAY
		//fprintf(fichier, "%f\n", speed_real);

		///********************************************************///	REGULATION
		erreur = CONSIGNE - speed_real;
		///*************************************///	Fourchette
		/*if( speed_real < CONSIGNE ) 	if( commande < COMMANDE_MAX ) commande += COMMANDE_INC;
		else							if( commande > COMMANDE_MIN ) commande -= COMMANDE_INC; */
		
		///*************************************///	P			
		//commande += Kp * erreur;
		commande += KPp * erreur;

		///*************************************///	PI
		//commande = commande_prev + KPIp*(erreur-erreur_prev) + KPIi*Te*erreur;
		
		///*************************************///	PID
		//commande = commande_prev + KPIDp*(erreur-erreur_prev) + KPIDi*Te*erreur + (KPIDd/Te)*(erreur - 2.0*erreur_prev + erreur_prev_prev);
		
		///*************************************///	Ecretage
		if( commande < COMMANDE_MIN ) commande = COMMANDE_MIN;
		if( commande > COMMANDE_MAX ) commande = COMMANDE_MAX;
		
		pwmWrite (1, commande) ;
		
		printf("Speed : %.1f\tSpeed2 : %.1f\t\tCmd : %.1f\t\tRef : %.1f\n", speed_real, speed_real_low, commande, CONSIGNE);
		
		commande_prev = commande;
		
		erreur_prev = erreur;
		erreur_prev_prev = erreur_prev;
		
		delay(PERIODE_ECH);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_2(void *arg)
{
	char prev;
	long int a, tab_mean[MEAN]={0}, tab_median[MEAN]={0}, tmp;
	int i, j, middle;
	long int sum;
	float temp;
	
	b = 0;		// 1
	c = 0;		// 0
	prev = 1;
	
	speed_real = 0.0;
	temp = 0.0;

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
			if( temp == 0.0 ) speed_real = 0.0;
			else speed_real = (float)GAIN_TEMPO/(1.0*temp);
		}
		
		if( a==1 ) 	b++;
		else 		c++;	
		
		prev = a;
		delayMicroseconds(LOOP_DURATION);
	
	}while(1);
	
	
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_3(void *arg)
{
	char prev;
	long int a, tab_mean[MEAN]={0}, tab_median[MEAN]={0}, tmp;
	int i, j, middle;
	long int sum;
	float temp;
	
	b = 0;		// 1
	c = 0;		// 0
	prev = 1;
	
	speed_real_low = 0.0;
	temp = 0.0;

	do
	{
		a = digitalRead(23);
		
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
			if( temp == 0.0 ) speed_real_low = 0.0;
			else speed_real_low = (float)GAIN_TEMPO*GAIN_TEMPO_LOW/(1.0*temp);
		}
		
		if( a==1 ) 	b++;
		else 		c++;	
		
		prev = a;
		delayMicroseconds(LOOP_DURATION);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}