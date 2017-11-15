#include <iostream>
#include <sstream>
#include <vector>
#include <bitset>
using namespace std;

const int BITS_IN_KEY = 56;
const int BITS_IN_CHAR = 8;

void printVector(vector<bool> inputVec)
{
  //Prints each character
  for (unsigned int i = 0; i < inputVec.size(); i++)
  {
    cout << inputVec.at(i);
    //Inserts a space between each group of 8
    if (((i + 1) % BITS_IN_CHAR == 0) && (i != 0))
    {
      cout << " ";
    }
  }
  cout << endl;
}

int main()
{
  char recMsg[] = {'6', '5'};
  string initKeyString(recMsg);
  int currentParityBit = 0;
  vector<bool> parityBits = {0, 0, 1, 1, 1, 1, 0, 0};
  int parityBitScale = parityBits.size();
  bitset<BITS_IN_KEY> initKeyBitset(stoi(initKeyString));

  string keyString = "";
  for (int i = BITS_IN_KEY - 1; i >= 0; i--)
  {
    char currentChar;
    if (((i + 1) % (parityBitScale - 1)) == 0 && (i != BITS_IN_KEY - 1))
    {
      currentChar =(parityBits.at(currentParityBit) ? '1' : '0');
      keyString += currentChar;
      currentParityBit++;
    }
    currentChar = (initKeyBitset[i] ? '1' : '0');
    keyString += currentChar;
  }
  char currentChar = (parityBits.at(parityBits.size() - 1) ? '1' : '0');
  keyString += currentChar;
  cout << keyString << endl;

  bitset<64> finalKeyBits(keyString);
  cout << "hi" << endl;
  cout << finalKeyBits << endl;
  //Successful receiving
  cout << "Key: " << finalKeyBits.to_ulong() << endl;
}