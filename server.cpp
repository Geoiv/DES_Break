#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <vector>
#include <time.h>
#include <bitset>

#include "DESBreakConsts.h"
#include "pthread_barrier.h"
using namespace std;

//Barrier to prevent the main thread from prematurely ending program
pthread_barrier_t threadBarrier;

const int FULL_KEY_BITS = 64;

//Data passed to each child thread
struct thread_data
{
   int threadId;
   int clientFileDesc;
};

//Listens for a success message from a particular client
//Ran be each child thread
void *listenForClient(void * threadArg)
{
  //Data passed to thread
  struct thread_data *threadData;
  threadData = (struct thread_data *) threadArg;

  //Waits for message from a specified client
  char recMsg[MAX_LINE];
  int receiveResult = recv(threadData->clientFileDesc, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    //No available messages
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    //Receiving failed
    cout << "Error receiving message from client" <<
      threadData->threadId << "." << endl;
    perror("Receiving message from client failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    string initKeyString(recMsg);
    int currentParityBit = 0;
    vector<bool> parityBits = {0, 0, 1, 1, 1, 1, 0, 0};
    int parityBitScale = parityBits.size();
    bitset<BITS_IN_KEY> initKeyBitset(stoi(initKeyString));
    string keyString = "";
    char currentChar;
    for (int i = BITS_IN_KEY - 1; i >= 0; i--)
    {
      if (((i + 1) % (parityBitScale - 1)) == 0 && (i != BITS_IN_KEY - 1))
      {
        currentChar =(parityBits.at(currentParityBit) ? '1' : '0');
        keyString += currentChar;
        currentParityBit++;
      }
      currentChar = (initKeyBitset[i] ? '1' : '0');
      keyString += currentChar;
    }
    currentChar = (parityBits.at(parityBits.size() - 1) ? '1' : '0');
    keyString += currentChar;

    bitset<FULL_KEY_BITS> finalKeyBits(keyString);

    //Successful receiving
    cout << "Key found by thread " << threadData->threadId << "!" << endl;
    cout << "Key: " << finalKeyBits.to_ulong() << endl;
    //Wait on the pthread barrier, allowing the main thread to continue running
    pthread_barrier_wait(&threadBarrier);
  }
  pthread_exit(NULL);
}

//Establishes a server socket and returns its file descriptor
int getServSocket()
{
  //Address struck for the server
  struct sockaddr_in servAddress;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddress.sin_port =  htons(PORT_NO);

  //Used to activate the option for address reuse
  const int reuse = 1;

  //Establishes the server file descriptor
  // AF_INET = Use IPv4,  SOCK_STREAM = Use TCP
  int servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (servFileDesc == -1)
  {
    perror("Server socket descriptor creation failed.");
    exit(EXIT_FAILURE);
  }
  //Sets address reuse options
  int setOptResults = setsockopt(servFileDesc, SOL_SOCKET,
                                 SO_REUSEADDR, &reuse, sizeof(reuse));
  if(setOptResults == -1)
  {
    close(servFileDesc);
    perror("Socket option setting failed.");
    exit(EXIT_FAILURE);
  }

  //Binds the socket file descriptor to an address and a port
  if (::bind(servFileDesc, (struct sockaddr *) &servAddress,
             sizeof(servAddress)) == -1)
  {
    close(servFileDesc);
    perror("Socket binding failed.");
    exit(EXIT_FAILURE);
  }

  //Sets the number of connections the socket should listen for
  if(listen(servFileDesc, MAX_CONNS) == -1)
  {
    //If listening fails
    close(servFileDesc);
    perror("Socket listening failed.");
    exit(EXIT_FAILURE);
  }
  return servFileDesc;
}

//Establishes connections with a set number of other machines in order to
//attempt a brute-force attack to break DES
int main()
{
  //Starts program execution timing
  clock_t startClock = clock();
  //Initializes the pthread barrier
  pthread_barrier_init(&threadBarrier, NULL, 2);

  //Address struct for clients
  struct sockaddr_in clientAddress;
  //Length of the client address
  socklen_t clientAddressLen;

  //Gets file descriptor for the server's socket
  int servFileDesc = getServSocket();

  //Vector to contain the file descriptors for the connected sockets
  vector<int> connectionFDs;

  //For each client
  for (int i = 0; i < CLIENT_COUNT; i++)
  {
    //Gets size of the client address
    clientAddressLen = sizeof(clientAddress);
    //Accepts connection attempts from client
    int connectFileDesc = accept(servFileDesc,
      (struct sockaddr*) &clientAddress, &clientAddressLen);

    connectionFDs.push_back(connectFileDesc);

    cout << "Connection request received..." << endl;
    //Sends the ID of the current client to the client
    const char* outgoingClientID = to_string(i).c_str();
    int sendResult = send(connectFileDesc, outgoingClientID,
      sizeof(outgoingClientID), 0);
    //Sending message failed
    if (sendResult == -1)
    {
      close(servFileDesc);
      for (int j = 0; j <= i; j++)
      {
        close(connectionFDs.at(j));
      }
      cout << "Issue in commmunication with client #" << i << "." << endl;
      perror("Sending Client ID failed.");
      exit(EXIT_FAILURE);
    }
  }


  //Number of listener threads needed
  int listenerThreadCount = connectionFDs.size();
  //Array of listener threads
  pthread_t threads[listenerThreadCount];
  //Data to be passed to threads
  struct thread_data threadData[listenerThreadCount];

  // Create a thread that will listen for the result of each client program.
  for (int i = 0; i < CLIENT_COUNT; i++)
  {
    threadData[i].threadId = i;
    threadData[i].clientFileDesc = connectionFDs.at(i);

    int resultCode = pthread_create(&threads[i], NULL, listenForClient,
                                    (void *)&threadData[i]);
    if (resultCode)
    {
       cout << "Error: unable to create thread, " << resultCode << endl;
       exit(-1);
    }
  }

  //Main thread waits until one of the CLIENT_COUNT child threads is done
  pthread_barrier_wait(&threadBarrier);
  pthread_barrier_destroy(&threadBarrier);

  // If something is recieved, the client found the right key.
  // Now tell all other clients to stop running.
  const char* endMessage = "STOP";
  for (unsigned short i = 0; i < connectionFDs.size(); i++)
  {
    // Stop all Client programs by sending them a message.
    int sendResult = send(connectionFDs.at(i), endMessage,
                          sizeof(endMessage), 0);
    if (sendResult == -1)
    {
      //If sending failed
      close(servFileDesc);
      for (int j = 0; j <= i; j++)
      {
        close(connectionFDs.at(j));
      }
      cout << "Issue telling client #" << i << " to stop executing." << endl;
      perror("Stopping client failed.");
      exit(EXIT_FAILURE);
    }
  }

  //Closes all sockets
  close(servFileDesc);
  for (unsigned int i = 0; i < connectionFDs.size(); i++)
  {
    close(connectionFDs.at(i));
  }
  //Gets total execution time for program
  clock_t timeElapsed = clock() - (float)startClock;
  cout << "Time elapsed for keyspace searching was " <<
    (float)((timeElapsed / (float)CLOCKS_PER_SEC))
    << " seconds." << endl;
  return EXIT_SUCCESS;
}