
#include <vector>
#include <string>
#include <cmath>

using namespace std;

const int PORT_NO = 3000;
const int MAX_LINE = 4096;
const int BITS_IN_KEY = 64;
const int CLIENT_COUNT = 1;
const short NUM_THREADS = 1;
const unsigned long long TOTAL_KEYS = pow(2, BITS_IN_KEY);
const short TOTAL_THREADS = NUM_THREADS * CLIENT_COUNT;
const unsigned long long THREAD_KEY_OFFSET = TOTAL_KEYS/TOTAL_THREADS;
// Have threads explore a few more keys than they're assigned to account for
// mathematical errors.
const unsigned short KEY_BUFFER = 3;