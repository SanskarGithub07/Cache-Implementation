#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

using namespace std;

// Cache block structure
struct CacheBlock {
    bool valid;
    unsigned int tag;
    vector<char> data;
    unsigned int counter; // Counter for LRU replacement policy

    CacheBlock(int block_size) : valid(false), tag(0), data(block_size), counter(0) {}
};

// Cache simulator class
class CacheSimulator {
private:
    int cache_size;      // Cache size in bytes
    int associativity;   // Associativity of the cache
    int block_size;      // Block size in bytes

    int num_blocks;      // Number of blocks in the cache
    int num_sets;        // Number of sets in the cache

    vector<vector<CacheBlock>> cache;  // 2D vector to represent the cache

    unsigned int time; // Global counter for LRU replacement policy

public:
    CacheSimulator(int size, int assoc, int bsize)
        : cache_size(size), associativity(assoc), block_size(bsize), time(0) {
        // Calculate the number of blocks and sets
        num_blocks = cache_size / block_size;
        num_sets = num_blocks / associativity;

        // Initialize the cache
        cache.resize(num_sets, vector<CacheBlock>(associativity, CacheBlock(block_size)));
    }

    // Function to perform a read operation
    void readOperation(unsigned int address) {
        unsigned int set_index = (address / block_size) % num_sets;
        unsigned int tag = address / (num_sets * block_size);

        // Search for the block in the set
        for (int i = 0; i < associativity; ++i) {
            if (cache[set_index][i].valid && cache[set_index][i].tag == tag) {
                // Cache hit
                cout << "Read Hit\t";
                for (char data : cache[set_index][i].data) {
                    cout << hex << setw(2) << setfill('0') << static_cast<int>(data);
                }
                cout << endl;

                // Update the counter for LRU policy
                cache[set_index][i].counter = time++;
                return;
            }
        }

        // Cache miss
        cout << "Read Miss" << endl;
    }

    // Function to perform a write operation
    void writeOperation(unsigned int address, const vector<char>& data) {
        unsigned int set_index = (address / block_size) % num_sets;
        unsigned int tag = address / (num_sets * block_size);

        // Search for the block in the set
        for (int i = 0; i < associativity; ++i) {
            if (cache[set_index][i].valid && cache[set_index][i].tag == tag) {
                // Cache hit for write
                cache[set_index][i].data = data;
                cache[set_index][i].counter = time++;

                cout << "Write Hit" << endl;
                return;
            }
        }

        // Search for an empty or invalid block in the set
        int emptyIndex = -1;
        int lruIndex = 0;
        unsigned int lruCounter = std::numeric_limits<unsigned int>::max();

        for (int i = 0; i < associativity; ++i) {
            if (!cache[set_index][i].valid) {
                // Found an empty block, update it
                cache[set_index][i].valid = true;
                cache[set_index][i].tag = tag;
                cache[set_index][i].data = data;
                cache[set_index][i].counter = time++;

                cout << "Write Miss" << endl;
                return;
            }

            // Track the least recently used block
            if (cache[set_index][i].counter < lruCounter) {
                lruCounter = cache[set_index][i].counter;
                lruIndex = i;
            }
        }

        // No empty block found, use LRU replacement policy
        cache[set_index][lruIndex].tag = tag;
        cache[set_index][lruIndex].data = data;
        cache[set_index][lruIndex].counter = time++;

        cout << "Write Miss (LRU Replacement)" << endl;
    }
};

int main() {
    int cache_size, associativity, block_size;
    
    cout << "Enter Cache Size (in bytes): ";
    cin >> cache_size;

    cout << "Enter Associativity: ";
    cin >> associativity;

    cout << "Enter Block Size (in bytes): ";
    cin >> block_size;

    CacheSimulator cacheSimulator(cache_size, associativity, block_size);

    char operation;
    unsigned int address;
    vector<char> data;

    cout << "Enter operations (R/W), Address (hex), Data (hex for write):" << endl;

    while (cin >> operation >> hex >> address) {
        if (operation == 'R') {
            cacheSimulator.readOperation(address);
        } else if (operation == 'W') {
            // For write operation, read the data as well
            data.resize(block_size);
            for (char& d : data) {
                cin >> hex >> d;
            }
            cacheSimulator.writeOperation(address, data);
        }
    }

    return 0;
}
