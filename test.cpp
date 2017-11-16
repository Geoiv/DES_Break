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

  int x = 5;
  cout << "1: " << x << endl;
  x = x>>1;
  cout << "2: " << x << endl;
  x = x<<1;
  cout << "3: " << x << endl;
}