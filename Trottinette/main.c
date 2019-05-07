#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#include "wiringPi.h"

#define MEAN 5
#define MEAN_COMMAND 3
#define CONSIGNE_INIT 5.0				// 12.0
#define COMMANDE_MIN 600				// 2.69V
#define COMMANDE_MAX 1023				// 4.40V
#define LAUNCH 700.0
		
#define COMMANDE_INC 1.0
#define MAX_COUNT 1000.0/15.0			// 1 tr/s
#define LOOP_DURATION 1.0
#define GAIN_TEMPO LOOP_DURATION * 1000.0 / 15.0
#define PERIODE_ECH 10
#define MAX_STR 10
#define Te 0.01
#define DERIV_MAX 300.0

// Min		Max
// 2.69V	4.4V
// 460		65

#define Kp 100.0		// 12.0
#define Ki 0.1		// 0.1
#define Kd 5.0		// 4.3

pthread_mutex_t mutex_update = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed_real = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);
void *thread_2(void *arg);
void *thread_3(void *arg);
void *thread_4(void *arg);
void *thread_5(void *arg);

float speed_real, erreur, erreur_prev, erreur_prev_prev, commande, commande_prev, consigne;
long int b, c;
char update, work;

char state = 1, state_previous = 1;		//	OFF = 1
										//	ON 	= 0;
FILE* fichier = NULL;

int main (void)
{
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;
	
	fichier = fopen("file.txt", "w+");
	
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
	commande = COMMANDE_MIN;
	commande_prev = COMMANDE_MIN;
	
	erreur = 0.0;
	erreur_prev = 0.0;
	erreur_prev_prev = 0.0;
	
	speed_real = 0.0;
			
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
	
	if(pthread_create(&thread5, NULL, thread_5, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
	}
	
	while(1);

	return 0;
}

void *thread_1(void *arg)
{
	float P = 0.0, I = LAUNCH, D = 0.0;

	//float tab_mean[MEAN_COMMAND] = {0.0}, sum, temp;

	//int i;
		
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
		
		///*************************************///	Fourchette
		//if( speed_real < consigne ) {	if( commande < COMMANDE_MAX ) commande += COMMANDE_INC; }
		//else			 				   {	if( commande > COMMANDE_MIN ) commande -= COMMANDE_INC; }
		
		///*************************************///	PID
		
		P = Kp * erreur;
				
		if( ( commande < COMMANDE_MAX && state == 0 ) || ( erreur < 0.0 && state == 0 ) )
		{
			I += Ki * Te * erreur;
		}

		D = (Kd / Te) * (erreur - erreur_prev);
      
      if( abs(D) > DERIV_MAX )
      {
         if ( D >= 0.0 )
         {
            D = DERIV_MAX;
         }
         else if ( D < 0.0 )
         {
            D = -1.0*DERIV_MAX;
         }
      }		

		commande = P + I + D;

		///*************************************///	Ecretage
		if( commande < COMMANDE_MIN ) commande = COMMANDE_MIN;
		if( commande > COMMANDE_MAX ) commande = COMMANDE_MAX;
					
		if( consigne == 0 )	commande = 0.0;
		
		//printf("Speed : %.1f tr/s\tCmd : %.0f\t\tP : %.0f\t\tI : %.0f\t\tD : %.0f\tRef : %.1f\t%d\n", speed_real, commande, P, I, D, consigne, state);

		/*sum = 0;
		
		for( i=0; i<MEAN_COMMAND-1; i++ )
		{
			tab_mean[i] = tab_mean[i+1];
			sum += tab_mean[i];	
		}
			
		tab_mean[MEAN_COMMAND-1] = commande;
		sum += tab_mean[MEAN_COMMAND-1];
			
		temp=(float)sum/MEAN_COMMAND;*/
				
		if(state == 0)
		{
			pwmWrite (1, (int)commande);
			//pwmWrite (1, (int)temp);
			
			//fprintf(fichier, "%.1f;%.1f;%.1f;%.1f;%.1f;%.1f\n", speed_real, temp, consigne, P, I, D);
		}
		else
		{
			pwmWrite (1, (int)COMMANDE_MIN) ;
		}

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
	long int a, tab_mean[MEAN]={0};//, tab_median[MEAN]={0}, tmp;
	int i;//, j, middle;
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
			
			sum += tab_mean[MEAN-1];
			temp=(float)sum/MEAN;
			
			///********************************************************///	MEDIAN
			/*for( i=0; i<MEAN; i++ ) tab_median[i] = tab_mean[i];
			
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
			
			temp = (float)tab_median[middle];*/
			
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
	do
	{		
		if (digitalRead(27)==0)
		{
			consigne += 1.0*COMMANDE_INC;
		}
		else
		{		
			if (digitalRead(26)==0)
			{
				consigne -= 1.0*COMMANDE_INC;
			}
		}
		
		if (consigne < COMMANDE_INC) consigne = COMMANDE_INC;

		delay(500);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_5(void *arg)
{
	//char chaine[MAX_STR];
	
	do
	{
		//printf("Quel est votre nom ? ");
		//fgets(chaine, MAX_STR, stdin);
		//printf("Vitesse: %s", chaine);
		
		//consigne = atof(chaine);
      
      delay(1000);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}