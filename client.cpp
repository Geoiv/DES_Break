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
const short NUM_THREADS = 12;
const int TOTAL_THREADS = NUM_THREADS * CLIENT_COUNT;
const unsigned long THREAD_KEY_OFFSET = TOTAL_KEYS/TOTAL_THREADS;
// Have threads explore a few more keys than they're assigned to account for
// mathematical errors.
const int BITS_IN_KEY = 64;

struct thread_data
{
   int threadId;
   int clientId;
   unsigned long startingKeyNum;
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
   unsigned int startingKeyNum = threadData->startingKeyNum;

   //Number of character groups that will need to be decrypted
   short charGroupCount = cipherText.size()/CHARS_IN_BLOCK;
   unsigned long threadKeyRange;

   if (threadData->clientId == (CLIENT_COUNT - 1) &&
       threadData->threadId == (NUM_THREADS - 1))
   {
      threadKeyRange = ULONG_MAX - startingKeyNum;
   }
   else
   {
      threadKeyRange = THREAD_KEY_OFFSET;
   }
   unsigned long endingKeyNum = startingKeyNum + threadKeyRange;
   cout << "Client: " << threadData->clientId << endl;
   cout << "Thread: " << threadData->threadId << endl;
   cout << "  Key range: " << threadKeyRange << endl;
   cout << "  Space Min: " << startingKeyNum << endl;
   cout << "  Space Max: " << endingKeyNum << endl;

   unsigned long currentKey;
   for (unsigned long i = 0; i < threadKeyRange; i++)
   {
      decryptResults = "";
      currentKey = startingKeyNum + i;
      //cout << endl << "Thread ID:  " << threadData->threadId << "  " <<
      //" Starting Key Number : " << startingKeyNum << "    " <<
      //" Current Key Number : " << startingKeyNum + i << "    " << endl;
      bitset<BITS_IN_KEY> keyBitset (currentKey);
      //cout << "keyBits: " << keyBitset << endl;
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

void parentMethod(int clientId, int cliSockFileDesc, vector<char> cipherText,
                  string plainText)
{
   pthread_t threads[NUM_THREADS];
   struct thread_data threadData[NUM_THREADS];

   // Client gives the ID - the order in which client connected to server
   // The ID acts as a multiplier(0-11)
   unsigned long parentKeySpaceStart = (TOTAL_KEYS / CLIENT_COUNT) * clientId;
   cout << endl << "parentKeySpaceStart:  " << parentKeySpaceStart << endl;

   for( int i = 0; i < NUM_THREADS; i++ )
   {
      cout <<"main() : creating thread, " << i << endl;
      threadData[i].threadId = i;
      threadData[i].clientId = clientId;
      threadData[i].startingKeyNum = parentKeySpaceStart + (i * THREAD_KEY_OFFSET);
      threadData[i].cliSockFileDesc = cliSockFileDesc;
      threadData[i].cipherText = cipherText;
      threadData[i].plainText = plainText;

      cout << "i: " << i << "  |  parentKeySpaceStart + " <<
         "(i * THREAD_KEY_OFFSET): " << parentKeySpaceStart +
         (i * THREAD_KEY_OFFSET) << endl;

      int resultCode = pthread_create(&threads[i], NULL, ThreadDecrypt, (
         void *)&threadData[i]);

      if (resultCode)
      {
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
  //File to read ciphertext from
  string cipherTextFileName = "ct.txt";
  //File to read plaintext from
  string plainTextFileName = "pt.txt";
  //Reads ciphertext as a vector of chars
  vector<char> cipherText = readInputAsVector(cipherTextFileName);
  //Reads the plaintext as a string
  string plainText = readInputAsString(plainTextFileName);

  //IP the client must connect to
  string targetIP;
  /*ONE MUST BE UNCOMMENTED FOR FUNCTIONING PROGRAM
  "127.0.0.1" IS FOR LOCALHOST TESTING*/

  //targetIP = "150.243.146.141";
  targetIP = "127.0.0.1";

  //File descriptor for this client's socket
  int cliSockFileDesc;

  //Address struct for this client
  struct sockaddr_in clientAddress;
  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(targetIP.c_str());
  clientAddress.sin_port =  htons(PORT_NO);
  //C-style string to recieve messages from server in
  char recMsg[MAX_LINE];
  //ID for this client (0 through CLIENT_COUNT - 1)
  int clientId;

  //Establishes the file descriptor for the socket
  // AF_INET = Use IPv4,  SOCK_STREAM = Use TCP
  cliSockFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (cliSockFileDesc == -1)
  {
    //Socket initialization failed
    perror("Socket descriptor creation failed.");
    exit(EXIT_FAILURE);
  }

  //Connection of the client to the server
  if (connect(cliSockFileDesc, (struct sockaddr *) &clientAddress,
    sizeof(clientAddress)) == -1)
  {
    //Connection failed
    close(cliSockFileDesc);
    perror("Problem in connecting to the server");
    exit(EXIT_FAILURE);
  }

  //Receives message from server containing the clientId
  int receiveResult = recv(cliSockFileDesc, recMsg, MAX_LINE, 0);
  if (receiveResult == 0)
  {
    //No messages available
    cout << "No available messages, or connection gracefully closed. " << endl;
  }
  else if (receiveResult == -1)
  {
    //Receiving message failed
    perror("Receiving message failed.");
    exit(EXIT_FAILURE);
  }
  else
  {
    //Receiving successful, gets clientId
    clientId = atoi(recMsg);

    //Begins the method for the parent thread to create the child threads
    parentMethod(clientId, cliSockFileDesc, cipherText, plainText);
  }

  //Closes the clients socket
  close(cliSockFileDesc);
  return EXIT_SUCCESS;
}