#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;


int main()
{
  vector<int> socketFDs;
  int sockFileDesc;
  string targetIP = "150.243.146.141";
  struct sockaddr_in servAddress;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = inet_addr(targetIP.c_str());
  servAddress.sin_port =  htons(PORT_NO);

  char* sendMsg = "Hello team member!";
  char* recMsg;

  for (short i = 0; i < SERVER_COUNT; i++)
  {
    //TODO protocol?
    sockFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    socketFDs.push_back(sockFileDesc);
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

    cout << "Message sending to server: " << sendMsg << endl;
    send(sockFileDesc, sendMsg, strlen(sendMsg), 0);
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
      cout << "Message received from server: " << recMsg << endl;
    }

  }

  bool stillLooping = false;
  while (stillLooping)
  {
    if (recv(sockFileDesc, recMsg, MAX_LINE, 0) == 0)
    {
      //error: server terminated prematurely
      perror("The server terminated prematurely");
      exit(EXIT_FAILURE);
    }
    cout << "String received from the server: " << recMsg << endl;
  }
  close(sockFileDesc);
  return EXIT_SUCCESS;
}
