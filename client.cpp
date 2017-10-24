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
  int servFileDesc;
  string targetIP;
  /*ONE MUST BE UNCOMMENTED FOR FUNCTIONING PROGRAM
  BOTTOM ONE IS FOR LOCALHOST TESTING*/
  //targetIP = "150.243.146.141";
  //targetIP = "127.0.0.1";

  //TODO is a new sockaddr_in required for each connection?
  struct sockaddr_in servAddress;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = inet_addr(targetIP.c_str());
  servAddress.sin_port =  htons(PORT_NO);
  const char* sendMsg = "Hello team member!";
  char recMsg[MAX_LINE];

  for (short i = 0; i < SERVER_COUNT; i++)
  {
    //TODO protocol?
    servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    socketFDs.push_back(servFileDesc);
    if (servFileDesc == -1)
    {
      perror("Socket descriptor creation failed.");
      exit(EXIT_FAILURE);
    }

    //Connection of the client to the socket
    if (connect(servFileDesc, (struct sockaddr *) &servAddress,
      sizeof(servAddress)) == -1)
    {
      perror("Problem in connecting to the server");
      close(servFileDesc);
      exit(EXIT_FAILURE);
    }

    cout << "Message sending to server: " << sendMsg << endl;
    int sendResult = send(servFileDesc, sendMsg, strlen(sendMsg), 0);
    if (sendResult == -1)
    {
      perror("Sending message failed.");
      exit(EXIT_FAILURE);
    }
    int receiveResult = recv(servFileDesc, recMsg, MAX_LINE, 0);
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
  close(servFileDesc);
  return EXIT_SUCCESS;
}
