#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "wiringPi.h"

void *thread_1(void *arg);
void *thread_2(void *arg);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared = 0;

int main (void)
{
	pthread_t thread1;
	pthread_t thread2;
	
	if(pthread_create(&thread1, NULL, thread_1, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	if(pthread_create(&thread2, NULL, thread_2, NULL) == -1) {
	perror("pthread_create");
	return EXIT_FAILURE;
    }
	
	do
	{
	printf("%d\n", shared);
		delay(1000);
	}while(1);
	
	return 0 ;
}

void *thread_1(void *arg)
{
	printf("Nous sommes dans le thread1\n");
	
	pthread_mutex_lock(&mutex);
	shared = 3;
	pthread_mutex_unlock(&mutex);
	delay(4000);

	do
	{
		
		
		//pthread_mutex_unlock(&mutex);
	
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}

void *thread_2(void *arg)
{
	printf("Nous sommes dans le thread2\n");
	
	delay(2000);
	
	pthread_mutex_lock(&mutex);
	shared = 2;
	pthread_mutex_unlock(&mutex);
    
	do
	{
		
		
		
	}while(1);
	
    (void) arg;
    pthread_exit(NULL);
}