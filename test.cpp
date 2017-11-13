#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

int main()
{
  //Output string of converted binary values to be returned
  string outputText = "";
  stringstream hexStream;
  //Loops for each character in the block
    //String to hold the binary of the current character
  string tempString = "";
  //Adds bits for current character to string

  //Puts bits in a bitset
  bitset<8> temp("00110000");
  bitset<8> temp2("00110001");
  //Adds converted character to the output text
  hexStream << hex << temp.to_ulong();
  hexStream << hex << temp2.to_ulong();

  cout << hexStream.str();
}