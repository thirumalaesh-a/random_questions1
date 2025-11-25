/*
MERGE OVERLAPPING INTERVALS

PROBLEM: Given a collection of intervals, merge all overlapping intervals
- Input: [[1,3], [2,6], [8,10], [15,18]]
- Output: [[1,6], [8,10], [15,18]]
- Explanation: [1,3] and [2,6] overlap, so merge into [1,6]

BRUTE FORCE APPROACH:
- Compare every interval with every other interval
- Merge if overlapping, repeat until no more merges possible
- Time Complexity: O(n³) - multiple passes through array
- Space Complexity: O(n) for result storage

OPTIMIZED APPROACH (Current Implementation):
- Sort intervals by start time
- Single pass through sorted intervals merging as we go
- Time Complexity: O(n log n) due to sorting
- Space Complexity: O(n) for result storage
*/

class Solution {
public:
    std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        // Get number of intervals
        int arrSize = intervals.size();
        
        // Sort intervals by start time (first element of each interval)
        // After sorting: [[1,3], [2,6], [8,10]] becomes sorted if not already
        // Time: O(n log n)
        sort(intervals.begin(), intervals.end());
        
        // Result vector to store merged intervals
        std::vector<std::vector<int>> ans;
        
        // Iterate through each interval
        for(int i = 0; i < arrSize; i++){
            // Case 1: First interval OR current interval doesn't overlap with last merged
            // No overlap when: last interval's end < current interval's start
            // Example: [1,3] and [5,7] don't overlap because 3 < 5
            if(ans.empty() || ans.back()[1] < intervals[i][0]){
                // Add current interval as is (no merge needed)
                ans.push_back(intervals[i]);
            } 
            // Case 2: Current interval overlaps with last merged interval
            // Overlap when: last interval's end >= current interval's start
            // Example: [1,4] and [3,6] overlap because 4 >= 3
            else{
                // Merge by extending the end of last interval
                // Take maximum of both ends to cover entire range
                // Example: merge [1,4] and [3,6] → [1,6]
                ans.back()[1] = max(ans.back()[1], intervals[i][1]);
                
                // Note: No need to update start time because intervals are sorted
                // The start of ans.back() is already the minimum start
                // That's why this line is commented out:
                // ans.back()[0] = min(ans.back()[0], intervals[i][0]);
            }
        }
        
        // Return merged intervals
        return ans;
    }
};

/*
EXAMPLE WALKTHROUGH:
Input: [[1,3], [2,6], [8,10], [15,18]]

After sorting: [[1,3], [2,6], [8,10], [15,18]] (already sorted)

Step-by-step:
1. i=0: ans is empty → add [1,3]
   ans = [[1,3]]

2. i=1: [2,6] overlaps with [1,3] (because 3 >= 2)
   → merge: [1, max(3,6)] = [1,6]
   ans = [[1,6]]

3. i=2: [8,10] doesn't overlap with [1,6] (because 6 < 8)
   → add [8,10]
   ans = [[1,6], [8,10]]

4. i=3: [15,18] doesn't overlap with [8,10] (because 10 < 15)
   → add [15,18]
   ans = [[1,6], [8,10], [15,18]]

Final result: [[1,6], [8,10], [15,18]]
*/