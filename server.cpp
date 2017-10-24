#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;


int main()
{

  int sockFileDesc;
  struct sockaddr_in servAddress;
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLen;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = INADDR_ANY;
  servAddress.sin_port =  htons(PORT_NO);
  char* sendMsg;
  char* recMsg;

  //TODO protocol?
  sockFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFileDesc == -1)
  {
    perror("Socket descriptor creation failed.");
    exit(EXIT_FAILURE);
  }

  if (!bind(sockFileDesc, (struct sockaddr*) &servAddress, sizeof(servAddress)))
  {
    perror("Socket binding failed.");
    close(sockFileDesc);
    exit(EXIT_FAILURE);
  }

  if(!listen(sockFileDesc, MAX_BACKLOG_CONNS))
  {
    close(sockFileDesc);
    perror("Socket listening failed.");
    exit(EXIT_FAILURE);
  }

  bool stillLooping = true;
  while (stillLooping)
  {
    clientAddressLen = sizeof(clientAddress);
    int connectFileDesc = accept(sockFileDesc,
      (struct sockaddr*) &clientAddress, &clientAddressLen);

    cout << "Connection request received..." << endl;
    int receiveResult = recv(sockFileDesc, recMsg, MAX_LINE, 0);
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
      sendMsg = recMsg;
      send(connectFileDesc, sendMsg, strlen(sendMsg), 0);
      stillLooping = false;
    }
    close(connectFileDesc);
  }
  close(sockFileDesc);
  return EXIT_SUCCESS;
}