/*
RANGE QUERY OPERATIONS ON SORTED POINTS

PROBLEM: Count points within distance d from query point(s)
- query: Count points within [qp-d, qp+d]
- query_and: Count points in intersection of two ranges
- query_or: Count points in union of two ranges

BRUTE FORCE APPROACH:
- Iterate through all points and check if within range
- Time Complexity: O(n) per query
- Space Complexity: O(1)

OPTIMIZED APPROACH (Current Implementation):
- Use binary search on sorted points array
- lower_bound: first element >= left boundary
- upper_bound: first element > right boundary
- Time Complexity: O(log n) per query
- Space Complexity: O(1)
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Count points within distance d from query_point
// Time: O(log n), Space: O(1)
int query(const std::vector<double>& points, double query_point, double d){
    // Calculate range boundaries [qp-d, qp+d]
    double left = query_point - d;
    double right = query_point + d;

    // Find first point >= left boundary
    auto low = std::lower_bound(points.begin(), points.end(), left);
    // Find first point > right boundary
    auto high = std::upper_bound(points.begin(), points.end(), right);
    
    // Distance between iterators = count of points in range
    return high - low;
}

// Count points in intersection of two ranges (AND operation)
// Time: O(log n), Space: O(1)
int query_and(std::vector<double>& points, double qp1, double qp2, double d){
    // Intersection is [max(left1, left2), min(right1, right2)]
    double left = std::max(qp1 - d, qp2 - d);
    double right = std::min(qp1 + d, qp2 + d);

    // If ranges don't overlap, intersection is empty
    if(left > right) return 0;

    // Binary search for intersection range
    auto low = std::lower_bound(points.begin(), points.end(), left);
    auto high = std::upper_bound(points.begin(), points.end(), right);
    return high - low;
}

// Count points in union of two ranges (OR operation)
// Uses inclusion-exclusion principle: |A ∪ B| = |A| + |B| - |A ∩ B|
// Time: O(log n), Space: O(1)
int query_or(std::vector<double>& points, double qp1, double qp2, double d){
    // Count points in range 1
    int count_A = query(points, qp1, d);
    // Count points in range 2
    int count_B = query(points, qp2, d);
    // Count points in intersection (avoid double counting)
    int count_intersection = query_and(points, qp1, qp2, d);
    
    // Apply inclusion-exclusion principle
    return count_A + count_B - count_intersection;
}

int main() {
    std::vector<double> points = {0.0, 1.0, 1.5, 2.0, 2.5};
    
    // Test single range query
    int result1 = query(points, 2.0, 1.0);
    std::cout << "query(2.0, 1.0): " << result1 << std::endl;
    
    // Test intersection query
    int result2 = query_and(points, 0.5, 2.0, 1.0);
    std::cout << "query_and(0.5, 2.0, 1.0): " << result2 << std::endl;
    
    // Test union query
    int result3 = query_or(points, 0.5, 2.0, 1.0);
    std::cout << "query_or(0.5, 2.0, 1.0): " << result3 << std::endl;
    
    return 0;
}