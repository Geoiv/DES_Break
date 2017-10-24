#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "DES.cpp"

const int PORT_NO = 3000;
const int MAX_BACKLOG_CONNS = 10;

int main()
{

  int sockFileDesc;
  struct sockaddr_in servAddress;
  bzero(&servAddress, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = INADDR_ANY;
  servAddress.sin_port =  htons(PORT_NO);
  string sendMsg;
  string recMsg;

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
    //close(sockFileDesc);
    exit(EXIT_FAILURE);
  }

  if(!listen(sockFileDesc, MAX_BACKLOG_CONNS))
  {
    //close(sockFileDesc);
    perror("Socket listening failed.");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in clientAddress;
  socklen_t cliAddrLen;

  while (true)
  {
    int connectedFileDesc = accept(sockFileDesc,
      (struct sockaddr*) &clientAddress, &cliAddrLen);

      /* perform read write operations ...
    read(ConnectFD, buff, size)
    */


    if (shutdown(connectedFileDesc, SHUT_RDWR) == -1)
    {
      perror("shutdown failed");
      //close(connectedFileDesc);
      //close(sockFileDesc);
      exit(EXIT_FAILURE);
    }
    //close(connectedFileDesc);
  }
  //close(sockFileDesc);
  return EXIT_SUCCESS;
}