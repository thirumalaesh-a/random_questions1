/*
HASHMAP IMPLEMENTATION USING SEPARATE CHAINING

PROBLEM: Implement a hash map (key-value store) with put, get, remove operations
- Handle collisions using separate chaining (linked lists)
- Support integer keys and values
- Operations should be efficient on average

APPROACH:
- Use array of linked lists (buckets) for collision handling
- Hash function: key % bucket_size to distribute keys
- Each bucket stores list of (key, value) pairs
- Time Complexity: O(1) average, O(n) worst case per operation
- Space Complexity: O(n) where n = number of stored elements
*/

#include <iostream>
#include <vector>
#include <list>

class HashMap{
private:
    // Array of linked lists: each bucket contains list of (key, value) pairs
    std::vector<std::list<std::pair<int, int>>> bucket;
    
    // Total number of buckets in hash table
    int size = 10000;
    
    // Hash function: maps key to bucket index
    // Returns: index in range [0, size-1]
    int getHashIdx(int key) {
        return key % size;
    }

public:
    // Constructor: initialize hash table with empty buckets
    HashMap() {
        bucket.resize(size);
    }
    
    // Insert or update key-value pair
    // If key exists: update its value
    // If key doesn't exist: add new pair to bucket
    // Time: O(1) average, O(n) worst case
    void put(int key, int value){
        // Find which bucket this key belongs to
        int idx = getHashIdx(key);
        
        // Get reference to the chain (linked list) at this bucket
        auto &chain = bucket[idx];
        
        // Search for existing key in this chain
        for(auto& [k, v] : chain){
            if(k == key){
                // Key found: update value
                v = value;
                return;
            }
        }
        
        // Key not found: add new (key, value) pair to end of chain
        chain.emplace_back(key, value);
    }

    // Retrieve value associated with key
    // Returns: value if key exists, -1 if not found
    // Time: O(1) average, O(n) worst case
    int get(int key){
        // Find which bucket this key belongs to
        int idx = getHashIdx(key);
        
        // Get reference to the chain at this bucket
        auto& chain = bucket[idx];
        
        // Empty bucket: key definitely doesn't exist
        if(chain.empty()) 
            return -1;
        
        // Search through chain for matching key
        for(auto& [k, v] : chain){
            if(k == key){
                // Key found: return associated value
                return v;
            }
        }
        
        // Key not found in chain
        return -1;
    }

    // Remove key-value pair from hash map
    // Does nothing if key doesn't exist
    // Time: O(1) average, O(n) worst case
    void remove(int key)
    {
        // Find which bucket this key belongs to
        int idx = getHashIdx(key);
        
        // Get reference to the chain at this bucket
        auto& chain = bucket[idx];
        
        // Iterate through chain to find key
        for(auto it = chain.begin(); it != chain.end(); it++){
            if(it->first == key){
                // Key found: remove this node from linked list
                chain.erase(it);
                return;
            }
        }
        // Key not found: nothing to remove
    }
};

/*
COLLISION HANDLING EXAMPLE:
Suppose size = 10 and we insert:
- put(5, 100):  idx = 5 % 10 = 5  → bucket[5] = [(5, 100)]
- put(15, 200): idx = 15 % 10 = 5 → bucket[5] = [(5, 100), (15, 200)]
- put(25, 300): idx = 25 % 10 = 5 → bucket[5] = [(5, 100), (15, 200), (25, 300)]

All three keys hash to bucket 5 (collision!), but separate chaining handles it.

get(15):
- idx = 15 % 10 = 5
- Search bucket[5]: skip (5, 100), find (15, 200)
- Return 200

WHY SEPARATE CHAINING?
- Simple to implement
- Never "full" - can always add more elements
- Performance degrades gracefully with collisions
- Alternative: open addressing (linear/quadratic probing)
*/

int main() {
    // Create hash map instance
    HashMap map;
    
    // Test put operation
    std::cout << "=== Testing put() ===" << std::endl;
    map.put(1, 100);
    map.put(2, 200);
    map.put(3, 300);
    std::cout << "Added: (1, 100), (2, 200), (3, 300)" << std::endl;
    
    // Test get operation
    std::cout << "\n=== Testing get() ===" << std::endl;
    std::cout << "get(1): " << map.get(1) << " (expected: 100)" << std::endl;
    std::cout << "get(2): " << map.get(2) << " (expected: 200)" << std::endl;
    std::cout << "get(99): " << map.get(99) << " (expected: -1, not found)" << std::endl;
    
    // Test update (put with existing key)
    std::cout << "\n=== Testing update ===" << std::endl;
    map.put(1, 999);
    std::cout << "Updated key 1 to 999" << std::endl;
    std::cout << "get(1): " << map.get(1) << " (expected: 999)" << std::endl;
    
    // Test remove operation
    std::cout << "\n=== Testing remove() ===" << std::endl;
    map.remove(2);
    std::cout << "Removed key 2" << std::endl;
    std::cout << "get(2): " << map.get(2) << " (expected: -1, removed)" << std::endl;
    
    // Test collision handling (keys that hash to same bucket)
    std::cout << "\n=== Testing collision handling ===" << std::endl;
    map.put(10001, 111);  // 10001 % 10000 = 1 (same bucket as key 1)
    std::cout << "Added key 10001 (collides with key 1)" << std::endl;
    std::cout << "get(1): " << map.get(1) << " (expected: 999)" << std::endl;
    std::cout << "get(10001): " << map.get(10001) << " (expected: 111)" << std::endl;
    
    return 0;
}