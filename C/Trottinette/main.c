﻿#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include "wiringPi.h"

// Slope detection
#define SIZE_TAB_PREDICT 300
#define GAIN_PREDICT 1000.0

// System parameters
#define K_osc 100.0
#define T_osc 10.0

// Ziegler & Nichols values for PID, with K_osc = 100 & T_osc = 10
#define Kp (0.6*K_osc)                                // 60

#define Ki (0.6*K_osc*2.0)/T_osc                      // 12
//#define Ki 10.0

#define Kd (0.6*K_osc*T_osc)/(8.0*SIZE_TAB_PREDICT)   // 75 => 0.375
//#define Kd 0.3

// Initial values integrator
#define A_I_Init 5.45
#define B_I_Init 639.0

// Integrator clipping
#define INTEGRATE_MAX 1000.0
#define INTEGRATE_MIN 300.0

// Derivator clipping
#define DERIV_MAX 300.0

#define MEAN 10
#define MEAN_COMMAND 1
#define CONSIGNE_INIT 5.0
#define COMMANDE_MIN 500            // 2.69V
#define COMMANDE_MAX 1023           // 4.40V
      
#define COMMANDE_INC 1.5
#define MAX_COUNT 1000.0/15.0       // 1 tr/s
#define LOOP_DURATION 1.0
#define GAIN_TEMPO LOOP_DURATION * 1000.0 / 15.0
#define POLLING_CHANGE_REFERENCE 100
#define MAX_STR 10
#define Te 0.005
#define MIN_SPEED 3.0

// Min      Max
// 2.69V    4.4V
// 460      65

pthread_mutex_t mutex_update = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_speed_real = PTHREAD_MUTEX_INITIALIZER;

void *thread_1(void *arg);
void *thread_2(void *arg);
void *thread_3(void *arg);
void *thread_4(void *arg);

float speed_real, erreur, erreur_prev, commande, consigne, P = 0.0, I = 0.0, D = 0.0;
float tab_predict[SIZE_TAB_PREDICT] = {0.0}, mean_predict = 0.0, sum_predict = 0.0;

long int b, c;

char start = 0;   //   OFF = 0
                  //   ON  = 1;

//FILE* fichier = NULL;

int main (void)
{
   pthread_t thread1;
   pthread_t thread2;
   pthread_t thread3;
   pthread_t thread4;
   
   //fichier = fopen("file.txt", "w+");
   
   if (wiringPiSetup () == -1)
   exit (1) ;

   pinMode (24, INPUT) ;            // Speed measurement
   
   //pinMode (3, OUTPUT) ;            // Start / Stop regulation
   //pullUpDnControl (3, PUD_DOWN);
   
   pinMode (27, INPUT) ;            // Increase speed
   pullUpDnControl (27, PUD_UP);
   
   pinMode (26, INPUT) ;            // Decrease speed
   pullUpDnControl (26, PUD_UP);
   
   pinMode (1, PWM_OUTPUT) ;        // DC Brushless control
   
   b = 0;      // 1
   c = 0;      // 0
   
   consigne = CONSIGNE_INIT; 
   commande = COMMANDE_MIN;
   
   erreur = 0.0;
   erreur_prev = 0.0;
   
   speed_real = 0.0;
   
   I = (A_I_Init * consigne) + B_I_Init;
         
   if(pthread_create(&thread1, NULL, thread_1, NULL) == -1)
   {
      perror("pthread_create");
      return EXIT_FAILURE;
   }
   
   if(pthread_create(&thread2, NULL, thread_2, NULL) == -1)
   {
      perror("pthread_create");
      return EXIT_FAILURE;
   }
   
   if(pthread_create(&thread3, NULL, thread_3, NULL) == -1)
   {
      perror("pthread_create");
      return EXIT_FAILURE;
   }
   
   if(pthread_create(&thread4, NULL, thread_4, NULL) == -1)
   {
      perror("pthread_create");
      return EXIT_FAILURE;
   }
   
   do
   {
      delay(100);
//      printf("%.0f\t%.0f\t%.0f\t%.1f\t%.1f\n",P,I,D,speed_real,consigne);
      
   }while(1);

   return 0;
}

void *thread_1(void *arg)
{
   int i;

   //float tab_mean[MEAN_COMMAND] = {0.0}, sum, temp;

   //int i;
      
   //printf("En attente du dÃ©marrage du moteur ...\n");
   //printf("\nKp : %f\t\tKi : %f\t\tKd : %f\n\n", Kp, Ki, Kd);
   
   ///********************************************************///   TEST DEAD LOCK
   do
   {
      if( b+c > MAX_COUNT )
      {
         //pthread_mutex_lock(&mutex_speed_real);
         speed_real = 0.0;
         //pthread_mutex_unlock(&mutex_speed_real);
      }

      ///********************************************************///   REGULATION
      erreur = consigne - speed_real;
      
      ///*************************************///   Fourchette
      //if( speed_real < consigne ) {   if( commande < COMMANDE_MAX ) commande += COMMANDE_INC; }
      //else                        {   if( commande > COMMANDE_MIN ) commande -= COMMANDE_INC; }
      
      ///*************************************///   PID
      
      P = Kp * erreur;
      
      sum_predict = 0.0;
      
      for( i=0; i<SIZE_TAB_PREDICT-1; i++ )
      {
         //sum_predict += tab_predict[i+1] - tab_predict[i];
         //sum_predict += tab_predict[i];
         tab_predict[i] = tab_predict[i+1];
      }

      for( i=0; i<SIZE_TAB_PREDICT-1; i++ )
      {
         sum_predict += tab_predict[i+1] - tab_predict[i];
      }
     
      tab_predict[SIZE_TAB_PREDICT-1] = erreur;
      
      mean_predict = GAIN_PREDICT * (sum_predict / (SIZE_TAB_PREDICT - 1.0));
      //mean_predict = sum_predict / (SIZE_TAB_PREDICT * 1.0);
      
      if( speed_real >= (consigne-1.0) )
      {
         start = 1;
      }
      else if( speed_real < MIN_SPEED )
      {
         start = 0;
         I = (A_I_Init * consigne) + B_I_Init;
      }
            
      if( (start == 1) && (commande < COMMANDE_MAX) )
      {
         I+= Ki * Te * erreur;
      }

      if ( I > INTEGRATE_MAX )
      {
         I = INTEGRATE_MAX;
      }
      else if ( I < INTEGRATE_MIN )
      {
         I = INTEGRATE_MIN;
      }

      //D = (Kd / Te) * (erreur - erreur_prev);
      D = (Kd / Te) * mean_predict;
      
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

      ///*************************************///   Ecretage
      if( commande < COMMANDE_MIN ) commande = COMMANDE_MIN;
      if( commande > COMMANDE_MAX ) commande = COMMANDE_MAX;
               
      if( consigne == 0 )   commande = 0.0;
      
      //printf("Speed : %.1f tr/s\tCmd : %.0f\t\tP : %.0f\t\tI : %.0f\t\tD : %.0f\tRef : %.1f\n", speed_real, commande, P, I, D, consigne);

      /*sum = 0;
      
      for( i=0; i<MEAN_COMMAND-1; i++ )
      {
         tab_mean[i] = tab_mean[i+1];
         sum += tab_mean[i];   
      }
         
      tab_mean[MEAN_COMMAND-1] = commande;
      sum += tab_mean[MEAN_COMMAND-1];
         
      temp=(float)sum/MEAN_COMMAND;*/
      
      pwmWrite (1, (int)commande);

      //fprintf(fichier, "%.1f;%.1f;%.1f;%.1f;%.1f;%.1f\n", speed_real, temp, consigne, P, I, D);
      
      erreur_prev = erreur;
      
      delay(Te * 1000.0);
   
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
   
   b = 0;      // 1
   c = 0;      // 0
   prev = 1;
   
   speed_real = 0.0;
   temp = 0.0;

   do
   {
      a = digitalRead(24);
      
      if ( prev==0 && a==1 )
      {      
         ///********************************************************///   MEAN
         sum = 0;
      
         for( i=0; i<MEAN-1; i++ )
         {
            tab_mean[i] = tab_mean[i+1];
            sum += tab_mean[i];   
         }
         
         tab_mean[MEAN-1] = b+c;
         
         sum += tab_mean[MEAN-1];
         temp=(float)sum/MEAN;
         
         ///********************************************************///   MEDIAN
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
      
         ///********************************************************///   UPDATE REAL SPEED         
         if( temp == 0.0 ) speed_real = 0.0;
         else speed_real = (float)GAIN_TEMPO/(1.0*temp);
      }
      
      if( a==1 )  b++;
      else        c++;   
      
      prev = a;
      delay(LOOP_DURATION);
   
   }while(1);

    (void) arg;
    pthread_exit(NULL);
}

void *thread_3(void *arg)
{
   float temp = 0.0;
   
   do
   {      
      if (digitalRead(27)==0)
      {
         delay(500);
         
         if (digitalRead(27)==0)
         {
            temp = consigne;
            temp += COMMANDE_INC;
            
            if( temp >= MIN_SPEED)
            {
               consigne = temp;
               I = (A_I_Init * consigne) + B_I_Init;
            }
            
            while(digitalRead(27)==0);
            delay(1000);
         }
      }
      else
      {      
         if (digitalRead(26)==0)
         {
            delay(500);
            
            if (digitalRead(26)==0)
            {
               temp = consigne;
               temp -= COMMANDE_INC;
               
               if( temp >= MIN_SPEED)
               {
                  consigne = temp;
                  I = (A_I_Init * consigne) + B_I_Init;
               }
               
               while(digitalRead(26)==0);
               delay(1000);
            }
         }
      }

      delay(POLLING_CHANGE_REFERENCE);
   
   }while(1);
   
    (void) arg;
    pthread_exit(NULL);
}

void *thread_4(void *arg)
{
   //char chaine[MAX_STR];
   //float consigne_temp, i, step;
   
   do
   {
      //printf("Quel est votre nom ? ");
      //fgets(chaine, MAX_STR, stdin);
      //printf("Vitesse: %s", chaine);
      
      //consigne = atof(chaine);
            
      delay(10);
   
   }while(1);
   
    (void) arg;
    pthread_exit(NULL);
}
