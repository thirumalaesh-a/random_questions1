/*
K CLOSEST POINTS TO ORIGIN

PROBLEM: Find k closest points to origin (0,0) using Euclidean distance
- Input: points = [[1,3], [-2,2], [5,8], [0,1]], k = 2
- Output: [[0,1], [-2,2]] or [[-2,2], [0,1]]
- Distance from origin: sqrt(x² + y²), but we can compare x² + y² directly

BRUTE FORCE APPROACH:
- Calculate distance for all points
- Sort all points by distance
- Return first k points
- Time Complexity: O(n log n) due to sorting all points
- Space Complexity: O(n) for storing distances

OPTIMIZED APPROACH (Current Implementation):
- Use max heap of size k to track k closest points
- Only keep k smallest distances in heap at any time
- Avoid sorting entire array
- Time Complexity: O(n log k) - process n points, heap operations take log k
- Space Complexity: O(k) for heap storage
*/

class Solution {
private:
    // Calculate squared distance from origin (0,0) to point
    // We use squared distance to avoid sqrt computation
    // sqrt(x² + y²) comparison same as x² + y² comparison
    // Time: O(1)
    int squaredDist(std::vector<int>& point){
        int x = point[0];  // x-coordinate
        int y = point[1];  // y-coordinate
        // Return x² + y² (Pythagorean theorem without sqrt)
        return x * x + y * y;
    }

public:
    std::vector<std::vector<int>> kClosest(std::vector<std::vector<int>>& points, int k) {
        // Max heap to store k closest points
        // Pair: {distance, index in points array}
        // Max heap keeps largest distance at top for easy removal
        std::priority_queue<std::pair<int,int>> maxHeap;

        // Process each point
        for(int i = 0; i < points.size(); i++){
            // Calculate squared distance and create entry
            int distance = squaredDist(points[i]);
            std::pair<int, int> entry = {distance, i};
            
            // Case 1: Heap not full yet, add point directly
            if(maxHeap.size() < k) {
                maxHeap.push(entry);
            } 
            // Case 2: Heap full, only add if current point is closer than farthest
            else if(entry.first < maxHeap.top().first){
                // Remove farthest point (top of max heap)
                maxHeap.pop();
                // Add closer point
                maxHeap.push(entry);
            }
            // If current point is farther than all k points in heap, ignore it
        }

        // Extract k closest points from heap
        std::vector<std::vector<int>> answer;
        while(!maxHeap.empty()){
            // Get index of point from heap
            int entryIndex = maxHeap.top().second;
            maxHeap.pop();
            // Add actual point to answer
            answer.push_back(points[entryIndex]);
        }
        
        return answer;
    }
};

/*
EXAMPLE WALKTHROUGH:
Input: points = [[3,3], [5,-1], [-2,4]], k = 2

Squared distances:
- [3,3]: 3² + 3² = 18
- [5,-1]: 5² + (-1)² = 26
- [-2,4]: (-2)² + 4² = 20

Step-by-step:
1. i=0: [3,3] dist=18, heap size < 2
   → push {18, 0}
   maxHeap = [{18, 0}]

2. i=1: [5,-1] dist=26, heap size < 2
   → push {26, 1}
   maxHeap = [{26, 1}, {18, 0}]  (26 at top)

3. i=2: [-2,4] dist=20, heap size = 2
   → 20 < 26 (top), so replace
   → pop {26, 1}, push {20, 2}
   maxHeap = [{20, 2}, {18, 0}]

Extract from heap:
- Pop {20, 2} → add [-2,4]
- Pop {18, 0} → add [3,3]

Result: [[-2,4], [3,3]]

WHY MAX HEAP?
- We want to quickly remove the FARTHEST point when we find a closer one
- Max heap keeps farthest point at top for O(log k) removal
- This maintains exactly k closest points efficiently
*/