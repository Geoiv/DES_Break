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


Aim: To design &  execute an experiment to break DES using 5 (or more) computers
 using brute force known plain text attack. Your strategy shall also include
 divide & conquer.

Platform: C/C++/Java/Python
Only one submission per team is required.

Part-2/Submission-A:
Step 1: Use 5 local computers. Divide the key space into 5 equal parts
(for each computer). And then each part would be subsequently divided into
another 5 equal parts (for each thread).

Step 2: Design a DES code breaking scheme by using brute force attack such that
each computer has exclusive key space to attack on (based on step 1). On each
computer create at least 5 threads using multithreading capability of the
programming platform. Each thread thus shall have an exclusive key space to
attack on.

Step 3: Your 5 computers shall be connected via network (you shall use TCP for
this purpose). This is because let's say if computer-1's thread-2 breaks the
code it shall inform all the other computers (and threads), from computer-2
till computer-5, so that they could stop trying. Since your key space is going
to be exclusive: if computer-1's thread-2 breaks the code then others will never
be able to and thus would be just wasting the valuable computing time.

Step 4: You will be provided with a. A plain text file 'P', b. A cipher text
file 'C'. The job of your scheme is to find the right key 'K' that was used to
encrypt file 'P' to file 'C'. This type of attack is typically called a known
plaintext attack. (I will provide these files on Oct 31 2017)
A word of advice: Try to use as little file I/O as possible because this will
slow down your scheme by an large factor. Also make sure your whole scheme is
breaking the DES code: you could use a few small keys for the purpose of testing.

What to submit: Report-A: A pdf file containing your entire source code,
description of your project that walks through your code and include details
like key distribution, port #s etc., and each team member contribution.

Part-2/Submission-B
Your submission must include: Report-B:
1. Plaintext
2. Ciphertext
3. Found Key 'K'
4. Time it took to break the key
5. Challenges Faced & Solutions Adopted
6. Learning Experience through this project
7. Team Dynamics
8. Feedback about this class (Optional).

Points: (Report A + Report B): 15 points.
Deadline: Oct 31 2017 for Part-2/Submission-A & November 27 2017 (tentative)
for Part-2/Submission-B.

Note: Your Part-2 submission will be graded after the deadline of
Part-2/Submission-B.

Extra Credit: (for 7.5 points)
+ You must use a cloud computing platform (like Amazon AWS, Microsoft Azure etc.)
+ Create atleast 7 instances of VMs (like AWS EC2) on your cloud platform.
+ These 7 instances would work just like an extension to your 5 primary
  computers created in Step-1 above.
+ Each of these instance must perform exactly the same brute force attack like
  your primary computers.
+ Your key space distribution would now be different since you would divide it
  into 12 equal parts (5 primary computers + 7 cloud computers), and then each
  part would be subsequently divided into another 5 equal parts (for each
  thread).
+ If you opt for extra credit, this work must be included in your
  Part-2/Submission-A & Part-2/Submission-B.
+ Feel free to email me if you have any questions.

