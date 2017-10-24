#include <iostream>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM_THREADS 5

struct thread_data {
   int  thread_id;
   long startingKeyNum;
};

void *PrintHello(void *threadarg) {
   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;

   cout << "Thread ID : " << my_data->thread_id << endl;
   cout << " Starting Key Number : " << my_data->startingKeyNum << endl;

   pthread_exit(NULL);
}


int main () {
   pthread_t threads[NUM_THREADS];
   struct thread_data td[NUM_THREADS];
   int resultCode;

   long totalKeys = pow(2, 56);
   cout << "totalKeys: " << totalKeys << endl;

   for (int i = 1; i <= 12; i++) {
      cout << (totalKeys / 12) * i << endl;
   }
   cout << totalKeys << endl;

   int myKeyspace = totalKeys / 12;

   for( int i = 0; i < NUM_THREADS; i++ ) {
      cout <<"main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].startingKeyNum = myKeyspace * i;
      resultCode = pthread_create(&threads[i], NULL, PrintHello, (void *)&td[i]);

      if (resultCode) {
         cout << "Error:unable to create thread," << resultCode << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
   return 0;
}