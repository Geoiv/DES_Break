#include <iostream>
#include <sstream>
#include <vector>
#include <bitset>
using namespace std;

int main()
{
  vector<char> inputVector = {'4', '1'};
  string currentPair;
  int pairVal;
  currentPair += inputVector.at(0);
  currentPair += inputVector.at(1);
  stringstream converterStream(currentPair);
  converterStream >> hex >> pairVal;
  bitset<8> temp(pairVal);
  cout << pairVal << endl;
  cout << temp << endl;


}