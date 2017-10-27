
#include <vector>
#include <string>
#include <cmath>

using namespace std;

const int PORT_NO = 3000;
const int MAX_LINE = 4096;
const int CLIENT_COUNT = 1;
const short NUM_THREADS = 1;
const long TOTAL_KEYS = pow(2, 56);
const short TOTAL_THREADS = NUM_THREADS * CLIENT_COUNT;
const long THREAD_KEY_OFFSET = TOTAL_KEYS/TOTAL_THREADS;
// Have threads explore a few more keys than they're assigned to account for
// mathematical errors.
const short KEY_BUFFER = 3;