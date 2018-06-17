#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

#define SENS 2
#define AVE 10
#define CONS 14
#define DELTA 3

void *thread_1(void *arg);
void *thread_2(void *arg);

long int cpt = 0;
int consigne = 500;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main (void)
{
	char str[10];
	long int a, b, c, prev, tour, cpt2;
	int tab[AVE] = {0}, i;
	long int sum;
	float temp;
	
	FILE* fichier = NULL;
	
	//fichier = fopen("file.txt", "w+");
	
	fichier = fopen("file.txt", "a");
	
	if (wiringPiSetup () == -1)
	exit (1) ;
	pinMode (24, INPUT) ;
	//pinMode (1, PWM_OUTPUT) ;
	//pwmWrite (1, 1024) ;
	pullUpDnControl (25, PUD_UP);
	
	
	//pullUpDnControl(25, PUD_DOWN);
	
	pthread_t thread1;
	pthread_t thread2;
	
	/*
	if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	if(pthread_create(&thread2, NULL, thread_2, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }*/
	
	b = 0;		// 1
	c = 0;		// 0
	prev = 1;
	tour = 0;
	cpt2=0;
	
	do
	{
		a = digitalRead(25);
		//fprintf(fichier, "%d\n", a);
		//fprintf(fichier, "ee\n");
		
		if ( prev==0 && a==1 )
		{
			
			sum = 0;
		
			for( i=0; i<AVE-1; i++ )
			{
				tab[i] = tab[i+1];
				sum += tab[i];
			}
			
			tab[AVE-1] = b+c;
			sum += tab[AVE-1];
			
			temp=(float)sum/AVE;
			//temp = b+c;
			
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
		
		tour++;
		
		if( tour >= 600 )
		{
			tour = 0;
			
			cpt2++;
			
			if (cpt2==20)
			{
				cpt2=0;
				printf("Vitesse : %.1f tr/s\n", (float)100000.0/(600.0*temp));
			}
		}

		//delay(1);
		delayMicroseconds(10);
		
		prev = a;
		
	}while(1);
	
	//fclose(fichier);

	return 0 ;
}

void detect_threshold()
{
	static int tab[SENS]={0};
	int i, test = 1;
		
	for (i=0; i<SENS; i++ )
	{			
		if( tab[i] == 0 ) test = 0;
	}
	
	if( (digitalRead(25)==LOW) && (test==1))
	{
		pthread_mutex_lock(&mutex);
		cpt++;
		pthread_mutex_unlock(&mutex);
		//printf("\n%ld", cpt);
	}
	
	for( i=0; i<SENS-1; i++ )
	{
		tab[i] = tab[i+1];
	}
	
	if( digitalRead(25)==HIGH )
	{
		tab[SENS-1]=1;
	}
	else
	{
		tab[SENS-1]=0;
	}
}

void *thread_1(void *arg)
{
    printf("Nous sommes dans le thread1\n");

	do
	{
		detect_threshold();
		
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_2(void *arg)
{
    int tab[AVE] = {0}, i;
	long int sum;
	
	printf("Nous sommes dans le thread2\n");
    
	do
	{
		delay(100);
		
		sum = 0;
		
		for( i=0; i<AVE-1; i++ )
		{
			tab[i] = tab[i+1];
			sum += tab[i];
		}
		
		tab[AVE-1] = cpt*10;
		sum += tab[AVE-1];
		
		printf("Vitesse : %.1f tr/s\n", (float)sum/AVE);
		/*
		if((float)sum/AVE < CONS) 	consigne += DELTA;
		else						consigne -= DELTA;
		
		pwmWrite (1, consigne) ;
		
		printf("Consigne : %d\n\n", consigne);*/
		
		pthread_mutex_lock(&mutex);
		cpt = 0;
		pthread_mutex_unlock(&mutex);
		
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}