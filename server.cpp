#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;

struct thread_data
{
   int threadId;
   int clientFileDesc;
   vector<int> allSockFDs;
};

void ListenForClient(void * threadArg)
{
  struct thread_data *threadData;
  threadData = (struct thread_data *) threadArg;

  char recMsg[MAX_LINE];
  int receiveResult = recv(threadData->clientID, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    perror("Receiving message from client failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    clientID = atoi(recMsg);
    parentMethod(clientID);
  }

  // If something is recieved, the client found the right key.
  // Now tell all other clients to stop running.
  if (recMsg[1] == '1')   // what should this condition be?
  {
    const char* endMessage = "Key found. Stop Running.";
    for (short i = 0; i < threadData->allSockFDs.size(); i++)
    {
      // Stop all Client programs by sending them a message.
      int sendResult = send(allSockFDs(i), endMessage, sizeof(endMessage), 0);
      if (sendResult == -1)
      {
        close(connectFileDesc);
        close(servFileDesc);
        cout << "Issue telling client #" << i << " to stop executing." << endl;
        perror("Stopping client failed.");
        exit(EXIT_FAILURE);
      }
    }
  }
  pthread_exit(NULL);
}

//TODO is a new sockaddr_in required for each connection?
int main()
{
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
    // TODO: add this description to Client
    // AF_INET = Use IPv4,  SOCK_STREAM = Use TCP
    int servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (servFileDesc == -1)
    {
      cout << "Issue in starting commmunication with client #" << i << "." <<
        endl;
      perror("Server socket descriptor creation failed.");
      exit(EXIT_FAILURE);
    }
    serverSockFDs.push_back(servFileDesc);
    //cout << "Server file descriptor created: " << servFileDesc << endl;

    if (::bind(servFileDesc, (struct sockaddr *) &servAddress, sizeof(servAddress)) == -1)
    {
      close(servFileDesc);
      cout << "Issue in starting commmunication with client #" << i << "." <<
        endl;
      perror("Socket binding failed.");
      exit(EXIT_FAILURE);
    }

    if(listen(servFileDesc, 1) == -1)
    {
      close(servFileDesc);
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
      close(connectFileDesc);
      close(servFileDesc);
      cout << "Issue in commmunication with client #" << i << "." << endl;
      perror("Sending Client ID failed.");
      exit(EXIT_FAILURE);
    }

    // close(connectFileDesc);
    // close(servFileDesc);
  }

  int listenerThreadCount = connectSockFDs.size();
  pthread_t threads[listenerThreadCount];
  struct thread_data threadData[listenerThreadCount];

  // Create a thread that will listen for the result of each client program.
  for (int i = 0; i < CLIENT_COUNT; i++)
  {
    threadData[i].threadID = i;
    threadData[i].clientFileDesc = connectFileDesc.at(i);
    threadData[i].allSockFDs = connectFileDesc;

    int resultCode = pthread_create(&threads[i], NULL, ListenForClient, (void *)&threadData[i]);
    if (resultCode)
    {
       cout << "Error:unable to create thread, " << resultCode << endl;
       exit(-1);
    }
  }

  for (unsigned int i = 0; i < serverSockFDs.size(); i++)
  {
    close(serverSockFDs.at(i));
  }
  return EXIT_SUCCESS;
}