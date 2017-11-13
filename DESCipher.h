/*
CS 455 - Project Part 1 - DES Implementation
Brandon Crane, Monica Singh, & George Wood
10/31/17
*/

#include <string>
#include <vector>

//Number of characters in each block of input text being processed
const short CHARS_IN_BLOCK = 8;
//Number of bits in a char variable
const short BITS_IN_CHAR = 8;
//Number of rounds the algorithm should operate for
const short ROUND_COUNT = 1;

class DESCipher
{
  private:
    //IP Table, takes in vector of size 64, permutes it, and modifies
    //left and right vectors passed by reference to give plaintext halves
    void initPerm(std::vector<bool> inputVector, std::vector<bool>& leftText,
      std::vector<bool>& rightText);

    //Inverse IP table, takes in vector of booleans of size 64 and returns
    //its permutation
    std::vector<bool> invInitPerm(std::vector<bool> inputVector);

    //PC1 Table, takes in vector of size 64, permutes it to size 56, and modifies
    //halves passed by reference to give initial key halves
    void pc1Perm(std::vector<bool> keyBits, std::vector<bool>& leftKey,
      std::vector<bool>& rightKey);

    //Key shift scheduler, takes in key halves and shifts them by a specified
    //number of bits based on the current round of encryption
    std::vector<bool> leftShiftSched(std::vector<bool> inputVector, short round);

    //PC2 Table, takes in key halves vectors of size 28 each, permutes them,
    //and returns a combined permuted key vector
    std::vector<bool> pc2Perm(std::vector<bool> leftKey, std::vector<bool> rightKey);

    //Takes in a vector of size 32, expands and permutes it with a hard-coded table,
    //and returns a vector of size 48
    std::vector<bool> eTablePerm(std::vector<bool> inputVector);

    //S Boxes, takes in vector of the right half of the plaintext of size 48
    //and returns a permuted and substituted vector of length 32
    std::vector<bool> sBoxSub(std::vector<bool> rightTextI);

    //Takes in vector of size 32, permutes it using a hard-coded table,
    //and returns another vector of size 32
    std::vector<bool> pTablePerm(std::vector<bool> inputVector);

  public:
    DESCipher();

    //Converts characters to bits and places them in a vector of bools
    static std::vector<bool> charsToBits(std::vector<char> inputVector);

    //Converts bits to chars and appends them to a string
    static std::string bitsToChars(std::vector<bool> inputVector);

    void printVector(std::vector<bool> inputVec);

    //Takes in a size 64 vector of booleans for both plaintext and the key
    //and performs DES encryption on them. Returns string of ciphertext for the
    //input block of plaintext
    std::string encrypt(std::vector<bool> plainTextBits, std::vector<bool> keyBits);

    //Takes in a size 64 vector of booleans for both plaintext and the key
    //and performs DES decryption on them. Returns string of plaintext for the
    //input block of ciphertext
    std::string decrypt(std::vector<bool> cipherTextBits, std::vector<bool> keyBits);
};