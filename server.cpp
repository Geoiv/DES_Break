#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;

const int CLIENT_COUNT = 1;

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
    //TODO protocol?
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

    close(connectFileDesc);
    close(servFileDesc);
  }

  for (unsigned int i = 0; i < serverSockFDs.size(); i++)
  {
    close(serverSockFDs.at(i));
  }
  return EXIT_SUCCESS;
}

/*
int reuse = 1;
if(setsockopt(servFileDesc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0);
{
  perror("Socket option setting failed.");
  exit(EXIT_FAILURE);
}
*/