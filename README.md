# DES

Explanation Document – CS 455 Group Assignment 1
Brandon Crane, Matt Frederick, Monica Singh, George Wood
9/18/17

TABLES
  IP Table and IP-1 Table (Initial Permutations)
    Create function that accepts 64 length array as a parameter
    Using table of permutations (represented by another 64 length array), loads elements of first array into permuted array         in a third array
    Returns third array
  E Table (Expansion & Permutation of right plaintext half)
    Same as IP Tables but with 32 length array and different permutation table
    Outputs 48 bits
  S-Boxes
    Uses 8 blocks of 6 bits from 48 bit input
    6 2D arrays, each 4 rows x 16 columns
    Apply each table to its corresponding block of 6 bits, ie. S1 for the first block, S2 for the second block
  P Table (Permutation of right plaintext half XOR’d with applied 48 bit key
    Same E Table but outputs 32 bits
  PC1-Table
    Takes 64 bit input key, removes 8 bits for 56 bit output
    Accomplished with 56 length array similar to above
  PC2-Table
    Same as PC-1 but takes 56 bits and outputs 48

I/O
  Encryption
    Read input plaintext and key from txt files
    Output crypttext to txt file
  Decryption
    Reads crypttext and key from txt files
    Outputs plaintext to txt file
KEY OPERATIONS
  Only make use of 56 bits, taken care of by PC-1 Table in first iteration
  Left shift key halves by the number of bits specified in the schedule
  Apply PC-2 to left shifted halves
PLAINTEXT OPERATIONS
  Store right plaintext half for use as next left plaintext half
  XOR it w/ PC-2 permuted key
  Apply S-Boxes to it
  Apply P-Table to it
  XOR mutated right half w/ unmutated left half, use result as next right half
AFTER ALL ROUNDS
  Final 32 bit shift
  Inverse IP table applied
DECRYPTION
  https://cstheory.stackexchange.com/questions/2543/how-can-des-have-6x4-s-boxes-and-still-be-reversible
  Largely the same as encryption, except:
    If encryption is:
      L(i+1) = R(i)
      R(i+1) = L(i)⊕F(R(i),K(i))
    Decryption will be:
      R(i) = L(i+1)
      L(i) = R(i+1)⊕F(L(i+1),K(i))
