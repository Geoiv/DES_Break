/*
CS 455 - Project Part 1 - DES Implementation
Brandon Crane, Matt Frederick, Monica Singh, & George Wood
9/30/17
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <time.h>

#include "DESCipher.h"
using namespace std;

//Reads input from a text file with specified name
vector<char> readInput(string inFileName)
{
  int charsRead = 0;
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
      charsRead++;
      readText.push_back(c);
    }
    inTextFileStream.close();
    //Gets number of characters that must be padded
    short charsToPad = CHARS_IN_BLOCK - (readText.size() % CHARS_IN_BLOCK);
    if (charsToPad != CHARS_IN_BLOCK)
    {
      for(short i = 0; i < charsToPad; i++)
      {
        readText.push_back('0');
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

//Writes output to a text file with a specified name
void writeOutput(string inputString, string fileName)
{
  ofstream outTextFileStream;
  outTextFileStream.open(fileName, std::ofstream::out | std::ofstream::trunc | std::ios::binary);
  //If file was successfully opened
  if (outTextFileStream.is_open())
  {
    //Writes each character to the file
    for(unsigned int i = 0; i < inputString.size(); i++)
    {
      outTextFileStream << inputString[i];
    }
    outTextFileStream.close();
  }
  //File could not be opened
  else
  {
    cout << "Could not write to file " << fileName << "." << endl;
    outTextFileStream.close();
  }
}

int main()
{
  //Filenames for input/output text files
  //Available ptFileNames are '8Char.txt', '8000Char.txt', and '80000Char.txt'.
  string ptFileName = "pt.txt";
  //string keyFileName = "key.txt";
  string encryptOutFileName = "encryptResults.txt";
  string decryptOutFileName = "decryptResults.txt";

  cout << "DES Cryptography Algoritm - CS 455 Project Part 1" << endl;

  DESCipher cipher;

  //Repeats until user wishes to quit
  bool stillLooping = true;
  while (stillLooping)
  {
    cout << "\nPlease enter \'e\' to encrypt, \'d\' " <<
      "to decrypt, or \'q\' to quit." << endl;
    //Gets user choice
    char mode;
    cin >> mode;
    //User wishes to encrypt
    if (mode == 'e')
    {
      //Starts timing program execution time
      clock_t startClock = clock();
      //Reads plaintext and key text
      vector<char> plainText = readInput(ptFileName);
      //vector<char> keyText = readInput(keyFileName);
      //If input text was of length 0 (such as when input fails)
      if ((plainText.size() == 0))
      {
        cout << "Read input of text or key was of length 0.\n";
        return 0;
      }
      //Key represented as bits
      vector<bool> keyBits;

      int keyInt = 65;
      bitset<64> keyIntBits(keyInt);
      for (int i = 63; i >= 0; i--)
      {
        keyBits.push_back(keyIntBits[i]);
      }
      //Number of character groups that will need to be encrypted
      short charGroupCount = plainText.size() / CHARS_IN_BLOCK;
      //Collects output ciphertext
      string cipherText = "";
      //Loops for each character group
      for (short i = 0; i < charGroupCount; i++)
      {
        //Holds characters of the current group
        vector<char> curGroupChars;
        //Current character group represented as bits
        vector<bool> curCharGroup;

        //Loads 8 characters into curGroupChars
        for (short j = 0; j < CHARS_IN_BLOCK; j++)
        {
          curGroupChars.push_back(plainText.at((i*CHARS_IN_BLOCK)+j));
        }
        //Converts current group to bit representation
        curCharGroup = DESCipher::charsToBits(curGroupChars);
        //Encrypts current group and appends output ciphertext to cipherText
        cipherText += cipher.encrypt(curCharGroup, keyBits);
      }
      //Writes output ciphertext to text file
      writeOutput(cipherText, encryptOutFileName);

      //Gets total execution time
      clock_t timeElapsed = clock() - (float)startClock;
      //Outputs results
      cout << "File encrypted to " << encryptOutFileName << "." << endl;
      cout << "Time elapsed for encryption was " <<
        (float)((timeElapsed / (float)CLOCKS_PER_SEC) * 1000)
        << " milliseconds." << endl;

    }
    //User wishes to decrypt
    else if (mode == 'd')
    {
      //Starts timing program execution time
      clock_t startClock = clock();
      //Reads ciphertext and key text
      vector<char> cipherText = readInput(encryptOutFileName);
      //vector<char> keyText = readInput(keyFileName);
      //If input text was of length 0 (such as when input fails)
      if ((cipherText.size() == 0))
      {
        return 0;
      }
      //Key represented as bits
      //Key represented as bits
      vector<bool> keyBits;

      int keyInt = 65;
      bitset<64> keyIntBits(keyInt);
      for (int i = 0; i < 64; i++)
      {
        keyBits.push_back(keyIntBits[i]);
      }

      //Number of character groups that will need to be decrypted
      short charGroupCount = cipherText.size()/CHARS_IN_BLOCK;
      //Collects output plaintext
      string plainText = "";
      //Loops for each character group
      for (short i = 0; i < charGroupCount; i++)
      {
        //Holds characters of the current group
        vector<char> curGroupChars;
        //Current character group represented as bits
        vector<bool> curCharGroup;
        //Loads 8 characters into curGroupChars
        for (short j = 0; j < CHARS_IN_BLOCK; j++)
        {
          curGroupChars.push_back(cipherText.at((i*CHARS_IN_BLOCK)+j));
        }
        //Converts current group to bit representation
        curCharGroup = DESCipher::charsToBits(curGroupChars);
        //Encrypts current group and appends output plaintext to plainText

        plainText += cipher.decrypt(curCharGroup, keyBits);
      }
      //Writes output ciphertext to text file
      writeOutput(plainText, decryptOutFileName);
      //Gets total execution time
      clock_t timeElapsed = clock() - (float)startClock;
      //Outputs results
      cout << "File decrypted to " << decryptOutFileName << "." << endl;
      cout << "Time elapsed for decryption was " <<
        (float)((timeElapsed / (float)CLOCKS_PER_SEC) * 1000)
        << " milliseconds." << endl;
    }
    //User wishes to quit
    else if (mode == 'q')
    {
      stillLooping = false;
    }
    //Invalid user input
    else
    {
      cout << "Input must either be \'e\', \'d\', or \'q\'. " <<
        "Please try again." << endl;
    }
  }
  return 0;
}
