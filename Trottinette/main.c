#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

#define MEAN 5		//5
#define CONSIGNE_INIT 6.0
#define COMMANDE_MIN 460				// 2.69V
#define COMMANDE_MAX 980			// 4.40V
#define LAUNCH 700
		
#define COMMANDE_INC 1
#define MAX_COUNT 10.0			// 1 tr/s
#define LOOP_DURATION 1.0
#define GAIN_TEMPO LOOP_DURATION * 1000.0 / 15.0
#define PERIODE_ECH 10
#define MAX_STR 10
#define Te 0.01

// Min		Max
// 2.69V	4.4V
// 		
// 460		65

#define KPp 100.0

//#define KPIi 100.0
//#de fine KPIp 3.0

#define KPIi 10.0
#define KPId 10.0
#define KPIp 1.0

pthread_mutex_t mutex_update = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed_real = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);
void *thread_2(void *arg);
void *thread_3(void *arg);
void *thread_4(void *arg);

float speed_real, erreur, erreur_prev, erreur_prev_prev, commande, commande_prev, consigne;
long int b, c;
char update, work;

char state = 1, state_previous = 1;		//	OFF = 1
										//	ON 	= 0;
int main (void)
{
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	
	if (wiringPiSetup () == -1)
	exit (1) ;

	pinMode (24, INPUT) ;				// Speed measurement
	
	pinMode (3, OUTPUT) ;				// Start / Stop regulatore
	pullUpDnControl (3, PUD_DOWN);
	
	pinMode (27, INPUT) ;				// Increase speed
	pullUpDnControl (27, PUD_UP);
	
	pinMode (26, INPUT) ;				// Decrease speed
	pullUpDnControl (26, PUD_UP);
	
	pinMode (1, PWM_OUTPUT) ;			// DC Brushless control
	
	b = 0;		// 1
	c = 0;		// 0
	
	consigne = CONSIGNE_INIT; 
	commande = LAUNCH;
	commande_prev = LAUNCH;
	
	erreur = 0.0;
	erreur_prev = 0.0;
	erreur_prev_prev = 0.0;
	
	speed_real = 0.0;
		
	//pwmWrite (1, commande);
	
	if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	if(pthread_create(&thread2, NULL, thread_2, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	if(pthread_create(&thread3, NULL, thread_3, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
	}
	
	if(pthread_create(&thread4, NULL, thread_4, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
	}
	
	while(1);

	return 0;
}

void *thread_1(void *arg)
{
	float I = LAUNCH , D; //P = 0.0, , D = 0.0;
	
	//printf("En attente du démarrage du moteur ...\n");
	//printf("\nKp : %f\t\tKi : %f\t\tKd : %f\n\n", KPIDp, KPIDi, KPIDd);
	
	///********************************************************///	TEST DEAD LOCK
	do
	{
		if( b+c > MAX_COUNT )
		{
			//pthread_mutex_lock(&mutex_speed_real);
			speed_real = 0.0;
			//pthread_mutex_unlock(&mutex_speed_real);
		}

		///********************************************************///	REGULATION
		erreur = consigne - speed_real;
		
		D = (erreur - erreur_prev ) * KPId;
		
		if( commande < COMMANDE_MAX && state == 0)	I += KPIi * Te * erreur;
		///*************************************///	Fourchette
		//if( speed_real < consigne ) 	{	if( commande < COMMANDE_MAX ) commande += COMMANDE_INC; }
		//else			 				{	if( commande > COMMANDE_MIN ) commande -= COMMANDE_INC; }
		
		///*************************************///	P			
		//commande += Kp * erreur;
		//commande = KPp * erreur;

		///*************************************///	PI
		//commande = commande_prev + KPIp*(erreur-erreur_prev) + KPIi*Te*erreur;
		
		commande = KPIp * ( erreur + I);
		
		///*************************************///	PID
		//commande = commande_prev + KPIDp*(erreur-erreur_prev) + KPIDi*Te*erreur + (KPIDd/Te)*(erreur - 2.0*erreur_prev + erreur_prev_prev);
		
		
		
		///*************************************///	Ecretage
		if( commande < COMMANDE_MIN ) commande = COMMANDE_MIN;
		if( commande > COMMANDE_MAX ) commande = COMMANDE_MAX;
		
		if( consigne == 0 )	commande = 0.0;
		
		//printf("Speed : %.0f tr/s\t\tCmd : %.0f\tErr : %.0f\t\tInt : %.0f\tRef : %.1f\t\t%d\n", speed_real, commande, erreur, I, consigne, state);
		
		//pwmWrite (1, consigne) ;
		//pwmWrite (1, 300) ;
		
		if(state == 0)
		{
			if(state_previous == 1)
			{
				pwmWrite (1, (300)) ;
				delay(200);
			}
			
			pwmWrite (1, (1024-commande)) ;

		}
		else
		{
			//if(state_previous == 0)
			//{	
				pwmWrite (1, 1000) ;
				//delay(3000);
			//}
		}
		
		//printf("Speed : %.0f tr/s\t\tCmd : %.0f\t\tRef : %.1f\t%d\n\n\n", speed_real, commande, consigne, state);

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
	//long int sum;
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
			//sum = 0;
		
			for( i=0; i<MEAN-1; i++ )
			{
				tab_mean[i] = tab_mean[i+1];
				//sum += tab_mean[i];	
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
		delay(LOOP_DURATION);
	
	}while(1);

    (void) arg;
    pthread_exit(NULL);
}

void *thread_3(void *arg)
{
	do
	{
		state_previous = state;
		
		if ( digitalRead(3)==1 )
		{
			state ^= 1;
			delay(500);
		}
		
		delay(10);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_4(void *arg)
{
	//char chaine[MAX_STR];
	
	do
	{
		/*fgets(chaine, MAX_STR, stdin);
		printf("Vitesse: %s", chaine);
		
		consigne = atof(chaine);*/
		
		if (digitalRead(27)==0)
		{
			consigne += 1.0;
		}
		else
		{		
			if (digitalRead(26)==0)
			{
				consigne -= 1.0;
			}
		}
		
		if (consigne < 1.0) consigne = 1.0;

		delay(500);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}