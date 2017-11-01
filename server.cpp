#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "DESBreakConsts.h"
using namespace std;

pthread_barrier_t threadBarrier;

struct thread_data
{
   int threadId;
   int clientFileDesc;
};

void listenForClient(void * threadArg)
{
  struct thread_data *threadData;
  threadData = (struct thread_data *) threadArg;

  char recMsg[MAX_LINE];
  int receiveResult = recv(threadData->clientFileDesc, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    cout << "Error receiving message from client" << threadData->threadId << "." << endl;
    perror("Receiving message from client failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    cout << "Key found by thread " << threadData->threadId << "!" << endl;
    cout << "Key: " << recMsg << endl;
    pthread_barrier_wait(&threadBarrier);
  }
  pthread_exit(NULL);
}

//TODO is a new sockaddr_in required for each connection?
int main()
{
  pthread_barrier_init(&threadBarrier, NULL, 2);

  struct sockaddr_in servAddress;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddress.sin_port =  htons(PORT_NO);

  struct sockaddr_in clientAddress;
  socklen_t clientAddressLen;

  vector<int> serverSockFDs;
  vector<int> connectSockFDs;
  for (int i = 0; i < CLIENT_COUNT; i++)
  {
    // AF_INET = Use IPv4,  SOCK_STREAM = Use TCP
    int servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (servFileDesc == -1)
    {
      for (int j = 0; j < i; j++)
      {
        close(j);
      }
      cout << "Issue in starting commmunication with client #" << i << "." <<
        endl;
      perror("Server socket descriptor creation failed.");
      exit(EXIT_FAILURE);
    }
    serverSockFDs.push_back(servFileDesc);
    //cout << "Server file descriptor created: " << servFileDesc << endl;

    if (::bind(servFileDesc, (struct sockaddr *) &servAddress, sizeof(servAddress)) == -1)
    {
      for (int j = 0; j <= i; j++)
      {
        close(serverSockFDs.at(j));
      }
      cout << "Issue in binding socket with client #" << i << "." <<
        endl;
      perror("Socket binding failed.");
      exit(EXIT_FAILURE);
    }

    if(listen(servFileDesc, 1) == -1)
    {
      for (int j = 0; j <= i; j++)
      {
        close(serverSockFDs.at(j));
      }
      cout << "Issue in starting commmunication with client #" << i << "." <<
        endl;
      perror("Socket listening failed.");
      exit(EXIT_FAILURE);
    }

    clientAddressLen = sizeof(clientAddress);
    int connectFileDesc = accept(servFileDesc,
      (struct sockaddr*) &clientAddress, &clientAddressLen);

    connectSockFDs.push_back(connectFileDesc);

    cout << "Connection request received..." << endl;
    const char* outgoingClientID = to_string(i).c_str();
    int sendResult = send(connectFileDesc, outgoingClientID,
      sizeof(outgoingClientID), 0);
    if (sendResult == -1)
    {
      for (int j = 0; j <= i; j++)
      {
        close(serverSockFDs.at(j));
        close(connectSockFDs.at(j));
      }
      cout << "Issue in commmunication with client #" << i << "." << endl;
      perror("Sending Client ID failed.");
      exit(EXIT_FAILURE);
    }
  }

  int listenerThreadCount = connectSockFDs.size();
  pthread_t threads[listenerThreadCount];
  struct thread_data threadData[listenerThreadCount];

  // Create a thread that will listen for the result of each client program.
  for (int i = 0; i < CLIENT_COUNT; i++)
  {
    threadData[i].threadId = i;
    threadData[i].clientFileDesc = connectSockFDs.at(i);

    int resultCode = pthread_create(&threads[i], NULL, listenForClient, (void *)&threadData[i]);
    if (resultCode)
    {
       cout << "Error: unable to create thread, " << resultCode << endl;
       exit(-1);
    }
  }

  pthread_barrier_wait(&threadBarrier);
  pthread_barrier_destroy(&threadBarrier);

  // If something is recieved, the client found the right key.
  // Now tell all other clients to stop running.
  const char* endMessage = "Key found. Stop Running.";
  for (unsigned short i = 0; i < connectSockFDs.size(); i++)
  {
    // Stop all Client programs by sending them a message.
    int sendResult = send(connectSockFDs.at(i), endMessage, sizeof(endMessage), 0);
    if (sendResult == -1)
    {
      for (int j = 0; j <= i; j++)
      {
        close(serverSockFDs.at(j));
        close(connectSockFDs.at(j));
      }
      cout << "Issue telling client #" << i << " to stop executing." << endl;
      perror("Stopping client failed.");
      exit(EXIT_FAILURE);
    }
  }

  for (unsigned int i = 0; i < serverSockFDs.size(); i++)
  {
    close(serverSockFDs.at(i));
    close(connectSockFDs.at(i));
  }
  return EXIT_SUCCESS;
}