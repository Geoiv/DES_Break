
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cmath>
#include <pthread.h>
#include <iostream>

#include "DESCipher.h"
#include "DESBreakConsts.h"
using namespace std;

struct thread_data {
   int  threadId;
   long startingKeyNum;
};

void *ThreadDecrypt(void *threadArg){
   struct thread_data *threadData;
   threadData = (struct thread_data *) threadArg;

   unsigned long currentKey;
   for (long i = 0; i < (THREAD_KEY_OFFSET + KEY_BUFFER); i++){
      currentKey = threadData->startingKeyNum + i;
      cout << endl << "Thread ID:  " << threadData->threadId << "  " <<
      " Starting Key Number : " << threadData->startingKeyNum << "    " <<
      " Current Key Number : " << threadData->startingKeyNum + i << "    " << endl;
      bitset<56> keyBits (currentKey);
      cout << "keyBits: " << keyBits << endl;

      // TODO call decrypt with the keyBits and the plaintext bits?
   }
   pthread_exit(NULL);
}

void parentMethod(int clientID)
{
   pthread_t threads[NUM_THREADS];
   struct thread_data threadData[NUM_THREADS];

   // Client gives the ID - the order in which client connected to server
   // The ID acts as a multiplier(0-11)
   long parentKeySpaceStart = (TOTAL_KEYS / CLIENT_COUNT) * clientID;
   cout << endl << "parentKeySpaceStart:  " << parentKeySpaceStart << endl;

   for( int i = 0; i < NUM_THREADS; i++ ) {
      cout <<"main() : creating thread, " << i << endl;
      threadData[i].threadId = i;
      threadData[i].startingKeyNum = parentKeySpaceStart + (i * THREAD_KEY_OFFSET);

      cout << "i: " << i << "  |  parentKeySpaceStart + (i * THREAD_KEY_OFFSET): " <<
      parentKeySpaceStart + (i * THREAD_KEY_OFFSET) << endl;

      int resultCode = pthread_create(&threads[i], NULL, ThreadDecrypt, (void *)&threadData[i]);

      if (resultCode) {
         cout << "Error:unable to create thread," << resultCode << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
}

int main()
{
  string targetIP;
  /*ONE MUST BE UNCOMMENTED FOR FUNCTIONING PROGRAM
  BOTTOM ONE IS FOR LOCALHOST TESTING*/
  //targetIP = "150.243.146.141";
  targetIP = "127.0.0.1";
  int cliSockFileDesc;

  struct sockaddr_in clientAddress;
  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(targetIP.c_str());
  clientAddress.sin_port =  htons(PORT_NO);
  char recMsg[MAX_LINE];
  int clientID;

  //TODO protocol?
  cliSockFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (cliSockFileDesc == -1)
  {
    perror("Socket descriptor creation failed.");
    exit(EXIT_FAILURE);
  }

  //Connection of the client to the socket
  if (connect(cliSockFileDesc, (struct sockaddr *) &clientAddress,
    sizeof(clientAddress)) == -1)
  {
    close(cliSockFileDesc);
    perror("Problem in connecting to the server");
    exit(EXIT_FAILURE);
  }

  int receiveResult = recv(cliSockFileDesc, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    perror("Receiving message failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    clientID = atoi(recMsg);
    parentMethod(clientID);
  }

  close(cliSockFileDesc);
  return EXIT_SUCCESS;
}

/*

*/
