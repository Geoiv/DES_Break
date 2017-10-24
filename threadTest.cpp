#include <iostream>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM_THREADS 5

struct thread_data {
   int  thread_id;
   char *message;
};

void *PrintHello(void *threadarg) {
   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;

   cout << "Thread ID : " << my_data->thread_id << endl;
   cout << " Message : " << my_data->message << endl;

   pthread_exit(NULL);
}

void *UseNum (void *threadArg){
   sleep(1);
   int *temp;
   temp = (int *) threadArg;
   cout << "Thread arg:" << temp << endl;
   pthread_exit(NULL);
}

int main () {
   pthread_t threads[NUM_THREADS];
   struct thread_data td[NUM_THREADS];
   int rc;
   char printMe[] = "Print me";
   int totalKeys = pow(2, 56);
   int numArray[5] = {128, 256, };

   for( int i = 0; i < NUM_THREADS; i++ ) {
      cout <<"main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].message = printMe;
      rc = pthread_create(&threads[i], NULL, PrintHello, (void *)&td[i]);
      rc = pthread_create(&threads[i], NULL, UseNum, (void *)&i);

      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
   return 0;
}