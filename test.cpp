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
  vector<bool> keyBits;
  int currentKey = 0;
  int currentParityBit = 0;
  vector<bool> parityBits = {0, 0, 1, 1, 1, 1, 0, 0};
  unsigned short parityBitScale = parityBits.size();
  bitset<BITS_IN_KEY> keyBitset(currentKey);
  for (int i = BITS_IN_KEY - 1; i >= 0; i--)
  {
    if (((i + 1) % (parityBitScale - 1)) == 0 && (i != BITS_IN_KEY - 1))
    {
      keyBits.push_back(parityBits.at(currentParityBit));
      currentParityBit++;
    }
    keyBits.push_back(keyBitset[i]);
  }
  keyBits.push_back(parityBits.at(parityBits.size() - 1));

  printVector(keyBits);


}