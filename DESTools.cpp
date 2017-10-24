
#include <vector>
#include <string>
using namespace std;


//Prints out binary values in a vector in separated groups of 8
//(For testing purposes)
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



//Converts characters to bits and places them in a vector of bools
vector<bool> charsToBits(vector<char> inputVector)
{
  vector<bool> outputVector;
  for (short i = 0; i < CHARS_IN_BLOCK; i++)
  {
    bitset<BITS_IN_CHAR> temp(inputVector.at(i));
    for(short j = BITS_IN_CHAR - 1; j >= 0; j--)
    {
      outputVector.push_back(temp[j]);
    }
  }
  return outputVector;
}

//Converts bits to chars and appends them to a string
string bitsToChars(vector<bool> inputVector)
{
  //Output string of converted binary values to be returned
  string outputText = "";
  //Loops for each character in the block
  for (short i = 0; i < CHARS_IN_BLOCK; i++)
  {
    //String to hold the binary of the current character
    string tempString = "";
    //Adds bits for current character to string
    for (short j = 0; j < BITS_IN_CHAR; j++)
    {
      tempString += (inputVector.at((i * CHARS_IN_BLOCK) + j) ? "1" : "0");
    }
    //Puts bits in a bitset
    bitset<BITS_IN_CHAR> temp(tempString);
    //Adds converted character to the output text
    outputText += (char)temp.to_ulong();

    if (VERBOSE)
    {
      cout << "Current char:\n" << char(temp.to_ulong()) << endl;
    }
  }
  return outputText;
}

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