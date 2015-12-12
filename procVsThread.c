#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define _REENTRANT
#define DCE_COMPAT

int prime(void);
void* threadPrime(void*);
int processes(void);
int threads(void);

int startRange;
int endRange;

pthread_mutex_t printfLock = PTHREAD_MUTEX_INITIALIZER;

/**
 * The main function of the program. This is where all the user inputs such as the starting,
 * and ending ranges of the prime number finder are to be specified as well as if the user wants
 * to use processes or threads to execute the prime number function with that specified range.
 */
int main (void) {
  char choice[0];
  int choiceCheck = 0; 

  
  printf("What starting number would you like to use for the prime number finder?\n");
  scanf("%d", &startRange);

  printf("What ending number would you like to use for the prime numer finder?\n");
  scanf("%d", &endRange);

   if (choiceCheck == 0) {
      printf("What would you like to use - processes or threads?(p/t) \n");
      scanf("%1s", choice);

      switch(choice[0]) {
         case 'p' :
            processes();
            choiceCheck = 1;
            break;
         case 't' :
            threads();
            choiceCheck = 1;
            break;
         case 'q' :
            exit(0);
            break;
         default :
            printf("Invalid choice (case sensitive) - p for processes, t for threads, or q to quit.\n");
      }
   }

   return 0; 
}


/**
 * This method is initialized to create exactly five children processes in a "fan" style from the parent.
 * The method will also create and open a file to log information to such as the start, end and total time taken
 * as well as the child process information.
 */
int processes(void){
   pid_t childpid = 0; 
   pid_t parentpid;
   int i;
   //Determines the number of processers
   int processNum = 5;
 
   //Open a new, empty file and open it for updates. 'w+' will discard previous concents of the file and start new.
   FILE* f = fopen("ProcessLog.txt","w+");
   
   //Hard coding the number of processes to be run (Parent process included)
   for (i = 0; i < processNum; i++) 
      //Check for unexpected results/errors (childpid = -1)
      if ((childpid = fork()) <=0)
         break; 
         //perror("Something went wrong with the child ID, it produced -1 \n");
         //return -1;

      //Check for PARENT processes (childpid > 0)
      if (childpid > 0) {
         /*fprintf(stderr, "PARENT - i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
              i, (long)getpid(), (long)getppid(), (long)childpid);*/
         //Make parent wait for the children processes to finish.
         for (i = 0; i < processNum; i++) {
            parentpid = wait();
         }
      }

      //Check for CHILDREN processes (childpid = 1)
      else {
         printf("Process: %d  Process ID: %ld  Parent ID: %ld  Child ID: %ld\n",
              i, (long)getpid(), (long)getppid(), (long)childpid);
         fprintf(f, "Process: %d  Process ID: %ld  Parent ID: %ld  Child ID: %ld\n",
              i, (long)getpid(), (long)getppid(), (long)childpid);    

         //Properly format and retrieve time.
         time_t rawtime;
         struct tm * timeinfo;
         time (&rawtime);
         timeinfo = localtime (&rawtime);

         //Prints start time to console
         printf ("Process %d start time: %d:%d:%d\n", i, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
         //printf("Start time of all the processes at time %s",  asctime(timeinfo));
         //Prints start time to log text file.
         fprintf(f, "Process %d start time: %d:%d:%d\n", i, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
         //fprintf(name, "Start time of the processes %d at time %s", asctime(timeinfo));

         //Start the timer
         clock_t start = clock(), diff;

         //Executes the prime number finder.
         prime();

         //Calculate the difference of start time and now.
         diff = clock() - start;
         double msec = diff / (double)CLOCKS_PER_SEC;

         //Refreshes the time to get the latest time rather than using the old, first declared time.
         time (&rawtime);
         timeinfo = localtime (&rawtime);
         //printf("End of Process %d  is %s", i, asctime(timeinfo));
         //fprintf(f, "End of Process %d is %s", i, asctime(timeinfo));
         printf ("Process %d end time: %d:%d:%d\n", i, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
         fprintf (f, "Process %d end time: %d:%d:%d\n", i, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

         //printf("Total time taken for processor %d is %d.%d\n", i, msec/1000, msec%1000);
         printf("Total time taken for process %d is %f seconds\n\n", i, msec);
         fprintf(f, "Total time taken for process %d is %f seconds\n\n", i, msec);  
      }
   //Closes file I/O when done.
   fclose(f);
}

/*
 * This method is initialized if a user selects to create exactly five threads to complete the prime number finder with. 
 * The method calls a separate threadPrime function due to the threads not being able to properly write to a log file
 * if calling the prime function regularly, thus requiring that intermediary function.
 */
int threads(void) {
  pthread_t thread1, thread2, thread3, thread4, thread5;

  //Creates the five threads each executing the prime number function
  pthread_create (&thread1, NULL, threadPrime, NULL);
  pthread_create (&thread2, NULL, threadPrime, NULL);
  pthread_create (&thread3, NULL, threadPrime, NULL);
  pthread_create (&thread4, NULL, threadPrime, NULL);
  pthread_create (&thread5, NULL, threadPrime, NULL);

  pthread_join (thread1, NULL);
  pthread_join (thread2, NULL);
  pthread_join (thread3, NULL);
  pthread_join (thread4, NULL);
  pthread_join (thread5, NULL);

  printf ("\n");
  return 0;
}

/**
 * This method is the main intensive mathematical computation used for both processes and threads in order to
 * measure performance and efficiency of both. The function computes and prints out all the prime numbers that 
 * in the specified range of numbers.
 */
int prime(void) {
   int is_prime, i , j;
   for(i = startRange; i <= endRange; i++){
   is_prime = 1;
   for(j = 2; j <= sqrt(i); j++){
     if(i % j == 0){
         is_prime = 0;
         break;
     }
     else
         is_prime = 1;
     }
   if(i < 2)
     is_prime = 0;
   if(is_prime == 1)
     printf("%4d ",i);
   }
}

/**
 * This method is the intermediary function required for the creation of a thread and its associated function
 * for the prime number function. This method is required since the thread creation has to be associated with a function
 * but there is then no room for the creation and logging of the log files.
 */
void* threadPrime(void* arg) {

   FILE* f = fopen("ThreadLog.txt","w+");
   fprintf(f,"");
   fclose(f);

   //Open a new, empty file and open it for updates. 'w+' will discard previous concents of the file and start new.
   f = fopen("ThreadLog.txt","a+");

   //Properly format and retrieve time.
   time_t rawtime;
   struct tm * timeinfo;
   time (&rawtime);
   timeinfo = localtime (&rawtime);

   sched_yield();

   pthread_mutex_lock (&printfLock);
   //Prints start time to console
   printf ("Thread start time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
   //Prints start time to log text file.
   fprintf(f, "Thread start time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

   //Start the timer
   clock_t start = clock(), diff;

   //Executes the prime number finder.
   prime();

   //Calculate the difference of start time and now.
   diff = clock() - start;
   double msec = diff / (double)CLOCKS_PER_SEC;

   //Refreshes the time to get the latest time rather than using the old, first declared time.
   time (&rawtime);
   timeinfo = localtime (&rawtime);
   printf ("Thread end time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
   fprintf (f, "Thread end time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

   //printf("Total time taken for processor %d is %d.%d\n", i, msec/1000, msec%1000);
   printf("Total time taken this thread is %f seconds\n\n", msec);
   fprintf(f, "Total time taken for this thread is %f seconds\n\n", msec); 
   pthread_mutex_unlock (&printfLock);

   //Closes file I/O when done.
   fclose(f);
   return NULL;
}
