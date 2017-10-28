#include <iostream>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <bitset>

using namespace std;

#define NUM_THREADS 5

struct thread_data {
   int  threadId;
   long startingKeyNum;
   long threadKeySpaceOffset;
};

// void *PrintHello(void *threadArg) {
//    struct thread_data *myData;
//    myData = (struct thread_data *) threadArg;
//
//    sleep(2);
//    cout << "Thread ID : " << myData->threadId << endl;
//    sleep(1);
//    cout << " Starting Key Number : " << myData->startingKeyNum << "    " << endl;
//
//    pthread_exit(NULL);
// }

void *ThreadDecrypt(void *threadArg){
   struct thread_data *threadData;
   threadData = (struct thread_data *) threadArg;

   // Have threads explore a few more keys than they're assigned to account for
   // mathematical errors.
   const short buffer = 3;

   unsigned long currentKey;
   for (long i = 0; i < (threadData->threadKeySpaceOffset + buffer); i++){
      currentKey = threadData->startingKeyNum + i;
      cout << endl << "Thread ID:  " << threadData->threadId << "  " <<
      " Starting Key Number : " << threadData->startingKeyNum << "    " <<
      " Current Key Number : " << threadData->startingKeyNum + i << "    " << endl;
      bitset<56> keyBits (currentKey);
      cout << "keyBits: " << keyBits << endl;

      // TODO call encrypt with the keyBits and the plaintext bits?
   }

   pthread_exit(NULL);
}


int main () {
   pthread_t threads[NUM_THREADS];
   struct thread_data threadData[NUM_THREADS];
   const long TOTAL_KEYS = pow(2, 56);
   const short TOTAL_THREADS = 60;
   short parentIndex = 0; // 0 - 11

   // Client gives the index - the number of the parent
   // The index acts as a multiplier(0-11) ---> n * (TOTAL_KEYS / 12)
   // Use zero for first parent.
   long parentKeySpaceStart = (TOTAL_KEYS / 12) * parentIndex;
   cout << endl << "parentKeySpaceStart:  " << parentKeySpaceStart << endl;

   // Calculates number of keys in between each threads' starting position
   long threadKeySpaceOffset = TOTAL_KEYS/TOTAL_THREADS;
   cout << "threadKeySpaceOffset:  " << threadKeySpaceOffset << endl;

   for( int i = 0; i < NUM_THREADS; i++ ) {
      cout <<"main() : creating thread, " << i << endl;
      threadData[i].threadId = i;
      threadData[i].startingKeyNum = parentKeySpaceStart + (i * threadKeySpaceOffset);
      threadData[i].threadKeySpaceOffset = threadKeySpaceOffset;

      cout << "i: " << i << "  |  parentKeySpaceStart + (i * threadKeySpaceOffset): " <<
      parentKeySpaceStart + (i * threadKeySpaceOffset) << endl;

      // resultCode = pthread_create(&threads[i], NULL, PrintHello, (void *)&threadData[i]);
      int resultCode = pthread_create(&threads[i], NULL, ThreadEncrypt, (void *)&threadData[i]);

      if (resultCode) {
         cout << "Error:unable to create thread," << resultCode << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
   return 0;
}