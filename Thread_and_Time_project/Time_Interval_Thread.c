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
      pthread_mutex_lock(&mtx_input);
      while(input_flag ==0)
      {
         pthread_cond_wait(&condition_input, &mtx_input);
      }
      pthread_mutex_unlock(&mtx_input);

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

      // ts.tv_nsec += X;
      // // Sleep X nanosecond
      // clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
      // clock_gettime(CLOCK_REALTIME, &current);
      snprintf(T, sizeof T, "%ld.%09ld offset: %ld.%09ld \n", current.tv_sec, current.tv_nsec, diff_sec, diff_nsec);

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
   
   while(1){
      pthread_mutex_lock(&mtx_sample);
      while(sample_flag ==0)
      {
         pthread_cond_wait(&condition_sample, &mtx_sample);
      }

      // Point to offset_data_X.txt
      fp_offset = fopen(file_name,"a");

      // Calculate offset
      // offset = ((double)ts.tv_sec + 1.0e-9*ts.tv_nsec) - ((double)ts_previous.tv_sec + 1.0e-9*ts_previous.tv_nsec);
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
         fprintf(file,"\n%ld.%09ld", diff_sec, diff_nsec);  
         
      }

      ts_previous.tv_nsec = current.tv_nsec;
      ts_previous.tv_sec  = current.tv_sec;

      // Save offset to specific file name
      // fp_offset = fopen(file_name,"a");
      if (fp_offset){
         fprintf(fp_offset, "%ld.%09ld\n", diff_sec, diff_nsec);
      }
      else{
         printf("Failed to open the file \n");
      }
      fclose(fp_offset);

      // Save date and time to "time_and_interval.txt"
      fp = fopen("time_and_interval.txt","a");
      if (fp){
         fputs(T,fp);
      }
      else{
         printf("Failed to open the file \n");
      }
      fclose(fp);
      
      // Return sample_flag value to continue save time value
      sample_flag = 0;

      pthread_mutex_unlock(&mtx_sample);

   }
}

void *Input_function(void *INPUT){

   while(1){
   // Read input from freq.txt 
   fp_X = fopen("freq.txt","r");
   fscanf (fp_X, "%d", &X);

   // Notify Sample thread about X frequency
   pthread_mutex_lock(&mtx_input);
   input_flag = 1;
   pthread_cond_signal(&condition_input);
   pthread_mutex_unlock(&mtx_input);

   // printf("%d \n",X);
   fclose(fp_X);
   }
}

int main(int argc, char** argv) { 
   pthread_t SAMPLE, LOGGING, INPUT;
   int X;
   int  iret1, iret2, iret3;

   // fp = fopen("time_and_interval.txt","w");
   printf("X frequency: %d", X);

   sprintf(file_name,"offset_data_%s.txt", argv[1]);

   // Create independent threads each of which will execute function   //
   iret1 = pthread_create( &INPUT, NULL, Input_function, (void*) &INPUT);
   iret2 = pthread_create( &SAMPLE, NULL, Sample_function, (void *)&SAMPLE);  
   iret3 = pthread_create( &LOGGING, NULL, Logging_function, (void*) &LOGGING);

   pthread_join( INPUT, NULL);
   pthread_join( SAMPLE, NULL);
   pthread_join( LOGGING, NULL);
   exit(0);
}

