#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;

int main()
{

  int servFileDesc;
  struct sockaddr_in servAddress;
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLen;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddress.sin_port =  htons(PORT_NO);
  char recMsg[MAX_LINE];

  //TODO protocol?
  servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (servFileDesc == -1)
  {
    perror("Socket descriptor creation failed.");
    exit(EXIT_FAILURE);
  }
  cout << "Server file descriptor created: " << servFileDesc << endl;

  /*
  int reuse = 1;
  if(setsockopt(servFileDesc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0);
  {
    perror("Socket option setting failed.");
    exit(EXIT_FAILURE);
  }
  */
  if (::bind(servFileDesc, (struct sockaddr *) &servAddress, sizeof(servAddress)) == -1)
  {
    perror("Socket binding failed.");
    close(servFileDesc);
    exit(EXIT_FAILURE);
  }

  if(listen(servFileDesc, MAX_BACKLOG_CONNS) == -1)
  {
    close(servFileDesc);
    perror("Socket listening failed.");
    exit(EXIT_FAILURE);
  }

  bool stillLooping = true;
  while (stillLooping)
  {
    clientAddressLen = sizeof(clientAddress);
    int connectFileDesc = accept(servFileDesc,
      (struct sockaddr*) &clientAddress, &clientAddressLen);

    cout << "Connection request received..." << endl;
    int receiveResult = recv(connectFileDesc, recMsg, MAX_LINE, 0);
    if (receiveResult == 0)
    {
      cout << "No available messages. " << endl;
    }
    else if (receiveResult == -1)
    {
      perror("Receiving message failed.");
      exit(EXIT_FAILURE);
    }
    else
    {
      cout << "Message received and resent to client: " << recMsg << endl;
      send(connectFileDesc, recMsg, receiveResult, 0);
      stillLooping = false;
    }
    close(connectFileDesc);
  }
  close(servFileDesc);
  return EXIT_SUCCESS;
}