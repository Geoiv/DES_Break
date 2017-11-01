#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cmath>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <limits.h>

#include "DESCipher.h"
#include "DESBreakConsts.h"
using namespace std;

const unsigned long TOTAL_KEYS = ULONG_MAX;
const short NUM_THREADS = 1;
const int TOTAL_THREADS = NUM_THREADS * CLIENT_COUNT;
const unsigned long THREAD_KEY_OFFSET = TOTAL_KEYS/TOTAL_THREADS;
// Have threads explore a few more keys than they're assigned to account for
// mathematical errors.
const int BITS_IN_KEY = 64;

struct thread_data
{
   int  threadId;
   long startingKeyNum;
   int cliSockFileDesc;
   vector<char> cipherText;
   string plainText;
};

void *ThreadDecrypt(void *threadArg)
{
   struct thread_data *threadData;
   threadData = (struct thread_data *) threadArg;

   DESCipher cipher;
   string decryptResults;
   vector<char> cipherText = threadData->cipherText;
   vector<bool> keyBits;

   //Number of character groups that will need to be decrypted
   short charGroupCount = cipherText.size()/CHARS_IN_BLOCK;

   unsigned long currentKey;
   for (unsigned long i = 0; i < (THREAD_KEY_OFFSET); i++)
   {
      decryptResults = "";
      currentKey = threadData->startingKeyNum + i;
      cout << endl << "Thread ID:  " << threadData->threadId << "  " <<
      " Starting Key Number : " << threadData->startingKeyNum << "    " <<
      " Current Key Number : " << threadData->startingKeyNum + i << "    " << endl;
      bitset<BITS_IN_KEY> keyBitset (currentKey);
      cout << "keyBits: " << keyBitset << endl;
      for(short j = BITS_IN_KEY - 1; j >= 0; j--)
      {
        keyBits.push_back(keyBitset[j]);
      }

      for (int j = 0; j < charGroupCount; j++)
      {
        //Holds characters of the current group
        vector<char> curGroupChars;
        //Current character group represented as bits
        vector<bool> curCharGroup;
        //Loads 8 characters into curGroupChars
        for (short k = 0; k < CHARS_IN_BLOCK; k++)
        {
          curGroupChars.push_back(cipherText.at((j*CHARS_IN_BLOCK)+k));
        }
        //Converts current group to bit representation
        curCharGroup = DESCipher::charsToBits(curGroupChars);
        //Encrypts current group and appends output plaintext to plainText

        decryptResults += cipher.decrypt(curCharGroup, keyBits);
      }
      if (decryptResults.compare(threadData->plainText) == 0)
      {
        const char* foundKey = to_string(currentKey).c_str();
        send(threadData->cliSockFileDesc, foundKey, sizeof(foundKey), 0);
      }
   }
   pthread_exit(NULL);
}

void parentMethod(int clientID, int cliSockFileDesc, vector<char> cipherText,
                  string plainText)
{
   pthread_t threads[NUM_THREADS];
   struct thread_data threadData[NUM_THREADS];

   // Client gives the ID - the order in which client connected to server
   // The ID acts as a multiplier(0-11)
   long parentKeySpaceStart = (TOTAL_KEYS / CLIENT_COUNT) * clientID;
   cout << endl << "parentKeySpaceStart:  " << parentKeySpaceStart << endl;

   for( int i = 0; i < NUM_THREADS; i++ )
   {
      cout <<"main() : creating thread, " << i << endl;
      threadData[i].threadId = i;
      threadData[i].startingKeyNum = parentKeySpaceStart + (i * THREAD_KEY_OFFSET);
      threadData[i].cliSockFileDesc = cliSockFileDesc;
      threadData[i].cipherText = cipherText;
      threadData[i].plainText = plainText;

      cout << "i: " << i << "  |  parentKeySpaceStart + (i * THREAD_KEY_OFFSET): " <<
      parentKeySpaceStart + (i * THREAD_KEY_OFFSET) << endl;

      int resultCode = pthread_create(&threads[i], NULL, ThreadDecrypt, (void *)&threadData[i]);

      if (resultCode) {
         cout << "Error:unable to create thread," << resultCode << endl;
         exit(-1);
      }

   }
   char recMsg[MAX_LINE];
   recv(cliSockFileDesc, recMsg, MAX_LINE, 0);
   cout << recMsg << endl;
}

vector<char> readInputAsVector(string inFileName)
{
  ifstream inTextFileStream;
  inTextFileStream.open(inFileName, std::ios::binary);
  //Vector to hold read characters
  vector<char> readText;
  //If file was succesfully opened
  if (inTextFileStream.is_open())
  {
    char c;
    //Pushes current char to readText
    while (inTextFileStream.get(c))
    {
      readText.push_back(c);
    }
    inTextFileStream.close();
    //Gets number of characters that must be padded
    short charsToPad = CHARS_IN_BLOCK - (readText.size() % CHARS_IN_BLOCK);
    if (charsToPad != CHARS_IN_BLOCK)
    {
      for(short i = 0; i < charsToPad; i++)
      {
        readText.push_back('x');
      }
    }
  }
  //File could not be opened
  else
  {
    cout << "The file " << inFileName << "was not able to be opened." << endl;
    inTextFileStream.close();
  }
  return readText;
}

string readInputAsString(string inFileName)
{
  ifstream inTextFileStream;
  inTextFileStream.open(inFileName, std::ios::binary);
  //Vector to hold read characters
  //string readText;
  //If file was succesfully opened
  if (inTextFileStream.is_open())
  {
    string readText((istreambuf_iterator<char>(inTextFileStream)),
                     istreambuf_iterator<char>());
    inTextFileStream.close();
    //Gets number of characters that must be padded
    short charsToPad = CHARS_IN_BLOCK - (readText.size() % CHARS_IN_BLOCK);
    if (charsToPad != CHARS_IN_BLOCK)
    {
      for(short i = 0; i < charsToPad; i++)
      {
        readText.push_back('x');
      }
    }
    return readText;
  }
  //File could not be opened
  else
  {
    cout << "The file " << inFileName << "was not able to be opened." << endl;
    inTextFileStream.close();
    return "File reading failed.";
  }
}

int main()
{
  string cipherTextFileName = "ct.txt";
  string plainTextFileName = "pt.txt";
  vector<char> cipherText = readInputAsVector(cipherTextFileName);
  string plainText = readInputAsString(plainTextFileName);

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

  // AF_INET = Use IPv4,  SOCK_STREAM = Use TCP
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
    parentMethod(clientID, cliSockFileDesc, cipherText, plainText);
  }

  close(cliSockFileDesc);
  return EXIT_SUCCESS;
}