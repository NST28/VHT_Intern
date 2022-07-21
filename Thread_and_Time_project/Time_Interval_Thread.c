#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
// #include "curr_time.h"

static int X;
static int flag;
static char T[60];
static int start = 1;
static float offset;
static char file_name[30];
static int loop = 1;

static long diff_sec;
static long diff_nsec;
static long nano_diff_sec = 0;
static long nano_diff_nsec = 0;


FILE *fp;
FILE *fp_X;
FILE *fp_offset;

pthread_mutex_t mtx_sample = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t  condition_sample = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mtx_input = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t  condition_input = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mtx_logging = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t  condition_logging = PTHREAD_COND_INITIALIZER;

static int sample_flag = 0;
static int input_flag = 0;
static int logging_flag = 0;

struct timespec ts = {0,0};
struct timespec current = {0,0};
// struct timespec current = {0,0};
struct timespec ts_previous = {0,0};


void *Sample_function(void * SAMPLE)
{

   // Read current date and time of the system
   clock_gettime(CLOCK_REALTIME, &ts);
   char buff[100];

   while(1)
   {

      // check if X frequency changed (Input)
      // pthread_mutex_lock(&mtx_input);
      // while(input_flag ==0)
      // {
      //    pthread_cond_wait(&condition_input, &mtx_input);
      // }
      // pthread_mutex_unlock(&mtx_input);

      ts.tv_nsec += X;

      if(ts.tv_nsec > 1000000000)
      {
         // temp = ts.tv_nsec;
         ts.tv_nsec = ts.tv_nsec - 1000000000;
         ts.tv_sec++;
      }
      clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &ts,NULL);

      // Save current time
      clock_gettime(CLOCK_REALTIME,&current);

      // return flag for Input
      input_flag = 0;

      // Create signal for LOGGING thread 
      pthread_mutex_lock(&mtx_sample);
      sample_flag = 1;
      pthread_cond_signal(&condition_sample);
      pthread_mutex_unlock(&mtx_sample);
   }
}

void *Logging_function(void *LOGGING){

   // printf("X frequency: %d", X);

   int i=0;
   
   while(1){
      pthread_mutex_lock(&mtx_sample);
      while(sample_flag ==0)
      {
         pthread_cond_wait(&condition_sample, &mtx_sample);
      }
      

      // Point to offset_data_X.txt
      

      // Calculate offset
      // offset = ((double)current.tv_sec + 1.0e-9*current.tv_nsec) - ((double)ts_previous.tv_sec + 1.0e-9*ts_previous.tv_nsec);
      // printf("%.9f seconds\n", offset);

      diff_sec = ((long) current.tv_sec) - ts_previous.tv_sec ;
      diff_nsec;

      if(ts_previous.tv_nsec != current.tv_nsec || ts_previous.tv_sec != current.tv_sec)
      {
         if(current.tv_nsec > ts_previous.tv_nsec)
         {
            diff_nsec = current.tv_nsec - ts_previous.tv_nsec;
         }
         else 
         {
            diff_nsec = 1000000000 + current.tv_nsec - ts_previous.tv_nsec ;
            diff_sec = diff_sec - 1;
         }
         // fprintf(file,"\n%ld.%09ld", diff_sec, diff_nsec);  
         
      }

      ts_previous.tv_nsec = current.tv_nsec;
      ts_previous.tv_sec  = current.tv_sec;   

      if (fp_offset){
         fprintf(fp_offset, "%d %ld%09ld \n", i, diff_sec, diff_nsec);
      }
      else{
         printf("Failed to open the file \n");
      }

      i++;
      

      // Save date and time to "time_and_interval.txt"
      if (fp){
         fprintf(fp, "%ld.%09ld offset: %ld.%09ld \n", current.tv_sec, current.tv_nsec, diff_sec, diff_nsec);
      }
      else{
         printf("Failed to open the file \n");
      }
      
      // Return sample_flag value to continue save time value
      sample_flag = 0;

      pthread_mutex_unlock(&mtx_sample);

   }
}

void *Input_function(void *INPUT){

   while(1){
   // Read input from freq.txt 
   fscanf (fp_X, "%d", &X);

   // Notify Sample thread about X frequency
   // pthread_mutex_lock(&mtx_input);
   // input_flag = 1;
   // pthread_cond_signal(&condition_input);
   // pthread_mutex_unlock(&mtx_input);
   // printf("%d \n",X);
   
   }
}

int main(int argc, char** argv) { 
   pthread_t SAMPLE, LOGGING, INPUT;
   int X;
   int  iret1, iret2, iret3;

   // fp = fopen("time_and_interval.txt","w");
   

   sprintf(file_name,"offset_data_%s.txt", argv[1]);

   fp_X = fopen("freq.txt","r");
   fp_offset = fopen(file_name,"a");
   fp = fopen("time_and_interval.txt","a");

   // Create independent threads each of which will execute function   //
   iret1 = pthread_create( &INPUT, NULL, Input_function, (void*) &INPUT);
   iret2 = pthread_create( &SAMPLE, NULL, Sample_function, (void *)&SAMPLE);  
   iret3 = pthread_create( &LOGGING, NULL, Logging_function, (void*) &LOGGING);

   pthread_join( INPUT, NULL);
   pthread_join( SAMPLE, NULL);
   pthread_join( LOGGING, NULL);

   fclose(fp);
   fclose(fp_offset);
   fclose(fp_X);

   exit(0);
}

