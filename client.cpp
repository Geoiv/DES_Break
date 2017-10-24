#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "DES.cpp"

const int PORT_NO = 3000;
const int MAX_LINE = 4096;

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

  //Connection of the client to the socket
 if (!connect(sockFileDesc, (struct sockaddr *) &servAddress,
   sizeof(servAddress)))
 {
   perror("Problem in connecting to the server");
   exit(EXIT_FAILURE);
 }

 while (true)
 {

   send(sockFileDesc, sendMsg, strlen(sendMsg), 0);

   if (recv(sockFileDesc, recMsg, MAX_LINE,0) == 0)
   {
     //error: server terminated prematurely
     perror("The server terminated prematurely");
     exit(EXIT_FAILURE);
   }
   cout << "String received from the server: " << recMsg << endl;
 }

 return EXIT_SUCCESS;
}
