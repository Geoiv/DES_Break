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
  //String to hold the binary of the current character
  int tempInt = 11;
  //Adds bits for current character to string

  stringstream hexStream;
  string hexTemp;
  //Puts bits in a bitset
  bitset<BITS_IN_CHAR> temp(tempInt);
  hexStream << hex << temp.to_ulong();
  hexTemp = hexStream.str();
  //cout << hexTemp << endl;
  //Adds converted character to the output text
  cout << hexTemp << " " << hexTemp.length() << endl;
  if (hexTemp.length() < 2)
  {
    hexTemp = "0" + hexTemp;
  }
  cout << hexTemp << endl;
  // bitset<8> temp("1");
  //
  // cout << temp << endl;
  // cout << hex << temp.to_ulong() << endl;
}