
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;

void parentMethod(int clientID)
{
  
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
  }

  close(cliSockFileDesc);
  return EXIT_SUCCESS;
}

/*

*/
