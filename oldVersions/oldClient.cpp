#include <iostream>

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "DES.cpp"
#include "DESBreakConsts.h"
using namespace std;

const int SERVER_COUNT = 1;

vector<char*> readIPAddrs()
{
  vector<char*> ipAddrs;
  int lineSize = 16;
  string inFileName = "ipAddrs.txt";

  ifstream inTextFileStream;
  inTextFileStream.open(inFileName);
  //If file was succesfully opened
  if (inTextFileStream.is_open())
  {
    char* currentIP;
    //Pushes current char to readText
    for (int i = 0; i < SERVER_COUNT &&
      inTextFileStream.getline(currentIP, lineSize); i++)
    {
      ipAddrs.push_back(currentIP);
    }
    inTextFileStream.close();
  }
  //File could not be opened
  else
  {
    cout << "The file " << inFileName << "was not able to be opened." << endl;
    inTextFileStream.close();
  }
  return ipAddrs;
}

int main()
{
  string targetIP;
  /*ONE MUST BE UNCOMMENTED FOR FUNCTIONING PROGRAM
  BOTTOM ONE IS FOR LOCALHOST TESTING*/
  //targetIP = "150.243.146.141";
  targetIP = "127.0.0.1";
  vector<char*> ipAddrs = readIPAddrs();



  vector<int> socketFDs;
  int servFileDesc;
  //TODO is a new sockaddr_in required for each connection?
  struct sockaddr_in servAddress;
  memset(&servAddress, 0, sizeof(servAddress));
  servAddress.sin_family = AF_INET;
  servAddress.sin_addr.s_addr = inet_addr(targetIP.c_str());
  servAddress.sin_port =  htons(PORT_NO);
  //char recMsg[MAX_LINE];

  for (unsigned int i = 0; i < ipAddrs.size(); i++)
  {
    //TODO protocol?
    servFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    socketFDs.push_back(servFileDesc);
    if (servFileDesc == -1)
    {
      cout << "Issue in client procedure for server #" << i << "." << endl;
      perror("Socket descriptor creation failed.");
      exit(EXIT_FAILURE);
    }

    //Connection of the client to the socket
    if (connect(servFileDesc, (struct sockaddr *) &servAddress,
      sizeof(servAddress)) == -1)
    {
      close(servFileDesc);
      cout << "Issue in client procedure for server #" << i << "." << endl;
      perror("Problem in connecting to the server");
      exit(EXIT_FAILURE);
    }

    const char* sendServID = to_string(i).c_str();
    cout << "Server ID sending to server: " << sendServID << endl;
    int sendResult = send(servFileDesc, sendServID, strlen(sendServID), 0);
    if (sendResult == -1)
    {
      close(servFileDesc);
      cout << "Issue in client procedure for server #" << i << "." << endl;
      perror("Sending message failed.");
      exit(EXIT_FAILURE);
    }

  }

  int receiveResult = recv(servFileDesc, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    cout << "Issue in client procedure for server #" << i << "." << endl;
    perror("Receiving message failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    cout << "Message received from server: " << recMsg << endl;
  }

  close(servFileDesc);
  return EXIT_SUCCESS;
}

/*

*/
