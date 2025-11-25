/*
CIRCULAR QUEUE IMPLEMENTATION USING ARRAY

PROBLEM: Implement a circular queue with fixed capacity
- enQueue: Add element to rear
- deQueue: Remove element from front
- Front: Get front element without removing
- Rear: Get rear element without removing
- Circular: When reaching end of array, wrap around to beginning

APPROACH:
- Use fixed-size array and track head position + count
- Circular indexing: (headIdx + offset) % capacity
- Track count to distinguish empty vs full
- Time Complexity: O(1) for all operations
- Space Complexity: O(k) where k = capacity
*/

#include <iostream>
#include <vector>

class MyCircularQueue{
private:
    std::vector<int> data;    // Fixed-size array to store elements
    int count;                 // Current number of elements in queue
    int capacity;              // Maximum number of elements
    int headIdx;               // Index of front element

public:
    // Constructor: initialize queue with capacity k
    MyCircularQueue(int k){
        capacity = k;
        data.resize(k);        // Allocate array of size k
        count = 0;             // Queue starts empty
        headIdx = 0;           // Front starts at index 0
    }

    // Add element to rear of queue
    // Returns: true if successful, false if queue is full
    // Time: O(1)
    bool enQueue(int value){
        // Check if queue is full
        if(isFull()) 
            return false;

        // Calculate rear position: (head + count) wraps around using modulo
        // Example: capacity=5, headIdx=3, count=2 → rear=(3+2)%5=0 (wraps to start)
        data[(headIdx + count) % capacity] = value;
        
        // Increment count (one more element in queue)
        count += 1;
        return true;
    }

    // Remove element from front of queue
    // Returns: true if successful, false if queue is empty
    // Time: O(1)
    bool deQueue(){
        // Check if queue is empty
        if(isEmpty()) 
            return false;

        // Move head forward (circular): next element becomes front
        headIdx = (headIdx + 1) % capacity;
        
        // Decrement count (one less element in queue)
        count -= 1;
        return true;
    }

    // Get front element without removing it
    // Returns: front element, or -1 if queue is empty
    // Time: O(1)
    int Front(){
        if(isEmpty()) 
            return -1;

        // Return element at head position
        return data[headIdx];
    }

    // Get rear element without removing it
    // Returns: rear element, or -1 if queue is empty
    // Time: O(1)
    int Rear(){
        if(isEmpty()) 
            return -1;

        // Calculate rear position: last valid element is at (head + count - 1)
        // Example: headIdx=2, count=3 → rear=(2+3-1)%5=4
        return data[(headIdx + count - 1) % capacity];
    }

    // Check if queue is empty
    // Returns: true if no elements, false otherwise
    // Time: O(1)
    bool isEmpty(){
        return count == 0;
    }
    
    // Check if queue is full
    // Returns: true if at capacity, false otherwise
    // Time: O(1)
    bool isFull() {
        return count == capacity;
    }
};

/*
CIRCULAR QUEUE VISUALIZATION:
Capacity = 5, indices: [0, 1, 2, 3, 4]

Initial state:
  headIdx=0, count=0
  [_, _, _, _, _]

After enQueue(1), enQueue(2), enQueue(3):
  headIdx=0, count=3
  [1, 2, 3, _, _]
   ^head    ^rear

After deQueue():
  headIdx=1, count=2
  [1, 2, 3, _, _]
      ^head ^rear

After enQueue(4), enQueue(5), enQueue(6):
  headIdx=1, count=5 (FULL)
  [6, 2, 3, 4, 5]
   ^rear  ^head

Notice how element 6 wrapped around to index 0!
This is the "circular" part - we reuse freed space at the beginning.

WHY USE COUNT?
- Distinguishes empty vs full: both have headIdx = tailIdx potentially
- Empty: count = 0
- Full: count = capacity
- Rear calculation: (headIdx + count - 1) % capacity
*/

int main() {
    std::cout << "=== CIRCULAR QUEUE TEST ===" << std::endl;
    std::cout << "Creating queue with capacity 3\n" << std::endl;
    
    // Create circular queue with capacity 3
    MyCircularQueue* queue = new MyCircularQueue(3);
    
    // Test enQueue operations
    std::cout << "enQueue(1): " << queue->enQueue(1) << " (expected: 1/true)" << std::endl;
    std::cout << "enQueue(2): " << queue->enQueue(2) << " (expected: 1/true)" << std::endl;
    std::cout << "enQueue(3): " << queue->enQueue(3) << " (expected: 1/true)" << std::endl;
    std::cout << "enQueue(4): " << queue->enQueue(4) << " (expected: 0/false, queue full)\n" << std::endl;
    
    // Test Rear and isFull
    std::cout << "Rear(): " << queue->Rear() << " (expected: 3)" << std::endl;
    std::cout << "isFull(): " << queue->isFull() << " (expected: 1/true)\n" << std::endl;
    
    // Test deQueue
    std::cout << "deQueue(): " << queue->deQueue() << " (expected: 1/true, removed 1)" << std::endl;
    std::cout << "Front(): " << queue->Front() << " (expected: 2, new front)\n" << std::endl;
    
    // Test circular behavior: enQueue after deQueue
    std::cout << "enQueue(4): " << queue->enQueue(4) << " (expected: 1/true, wraps to start)" << std::endl;
    std::cout << "Rear(): " << queue->Rear() << " (expected: 4, wrapped around)" << std::endl;
    std::cout << "Front(): " << queue->Front() << " (expected: 2, unchanged)\n" << std::endl;
    
    // Show final state
    std::cout << "=== FINAL STATE ===" << std::endl;
    std::cout << "Queue contains: 2, 3, 4 (in circular array)" << std::endl;
    std::cout << "Front element: " << queue->Front() << std::endl;
    std::cout << "Rear element: " << queue->Rear() << std::endl;
    
    // Clean up
    delete queue;
    
    return 0;
}