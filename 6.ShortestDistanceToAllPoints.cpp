/*
GEOMETRIC MEDIAN: FIND POINT MINIMIZING SUM OF DISTANCES

PROBLEM: Find point (x,y) that minimizes sum of Euclidean distances to all given points
- Also known as "Fermat point" or "geometric median"
- Different from centroid (which minimizes sum of SQUARED distances)
- Used in facility location, network design, clustering

BRUTE FORCE APPROACH:
- Try all possible points in a grid within bounds
- For each candidate point, calculate total distance to all points
- Keep track of point with minimum total distance
- Time Complexity: O(g² × n) where g = grid resolution, n = number of points
- Space Complexity: O(1)
- Problem: Very slow, requires defining search bounds and grid granularity

OPTIMIZED APPROACH (Current Implementation - Gradient Descent):
- Start at centroid (average position) as initial guess
- Use iterative hill climbing with decreasing step sizes
- At each step, try moving in 8 directions
- Accept move if it reduces total distance
- Decrease step size when no improvement found
- Time Complexity: O(k × 8 × n) where k = iterations (typically small)
- Space Complexity: O(1)
- Advantage: Converges quickly, no need to define search bounds
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Calculate Euclidean distance between two points
// Formula: sqrt((x2-x1)² + (y2-y1)²)
// Time: O(1)
double calcDistance(double x1, double y1, double x2, double y2){
    double dx = x1 - x2;
    double dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

// Calculate sum of distances from point (x,y) to all given points
// This is the objective function we want to minimize
// Time: O(n) where n = number of points
double getTotalDistance(std::vector<std::vector<int>>& points, double x, double y){
    double total = 0.0;
    
    // Sum distance from (x,y) to each point
    for(const auto& p : points){
        total += calcDistance(x, y, p[0], p[1]);
    }
    
    return total;
}

// ============================================================================
// BRUTE FORCE APPROACH (FOR REFERENCE - NOT IMPLEMENTED)
// ============================================================================
/*
std::vector<double> getMinDistancePointBruteForce(std::vector<std::vector<int>>& points) {
    // Find bounds of all points
    int minX = INT_MAX, maxX = INT_MIN;
    int minY = INT_MAX, maxY = INT_MIN;
    for(const auto& p : points){
        minX = std::min(minX, p[0]);
        maxX = std::max(maxX, p[0]);
        minY = std::min(minY, p[1]);
        maxY = std::max(maxY, p[1]);
    }
    
    // Try all points in grid with step size 0.1
    double step = 0.1;
    double bestX = 0, bestY = 0;
    double minDist = INFINITY;
    
    // Check every point in grid (VERY SLOW!)
    for(double x = minX; x <= maxX; x += step){
        for(double y = minY; y <= maxY; y += step){
            double dist = getTotalDistance(points, x, y);
            if(dist < minDist){
                minDist = dist;
                bestX = x;
                bestY = y;
            }
        }
    }
    
    return {bestX, bestY};
}
// Time: O(((maxX-minX)/step) × ((maxY-minY)/step) × n) = O(g² × n)
// Very slow and requires defining search bounds!
*/

// ============================================================================
// OPTIMIZED APPROACH: GRADIENT DESCENT WITH MULTI-RESOLUTION
// ============================================================================

// Find point that minimizes sum of distances using iterative refinement
// Returns: [x, y] coordinates of optimal point
// Time: O(k × 8 × n) where k = number of iterations (typically small)
std::vector<double> getMinDistancePoint(std::vector<std::vector<int>>& points)
{
    // Edge case: no points
    if (points.empty()) 
        return {0.0, 0.0};

    // Edge case: single point (optimal point is the point itself)
    if (points.size() == 1) 
        return {(double)points[0][0], (double)points[0][1]};

    // ========================================================================
    // STEP 1: Initialize starting position as centroid (geometric center)
    // Centroid is good initial guess for geometric median
    // ========================================================================
    double x = 0.0, y = 0.0;
    for(const auto& p : points){
        x += p[0];
        y += p[1];
    }
    x /= points.size();  // Average x-coordinate
    y /= points.size();  // Average y-coordinate
    
    // Calculate initial total distance
    double minDist = getTotalDistance(points, x, y);

    // ========================================================================
    // STEP 2: Define search directions (8 neighbors in grid)
    // ========================================================================
    // Directions: NW, N, NE, W, E, SW, S, SE
    std::vector<std::pair<int, int>> directions = {
        {-1, 1},   // Northwest
        {0, 1},    // North
        {1, 1},    // Northeast
        {-1, 0},   // West
        {1, 0},    // East
        {-1, -1},  // Southwest
        {0, -1},   // South
        {1, -1}    // Southeast
    };

    // ========================================================================
    // STEP 3: Multi-resolution search with decreasing step sizes
    // Start with large steps, progressively refine with smaller steps
    // ========================================================================
    std::vector<double> steps = {1.0, 0.1, 0.01, 0.001};

    for(double step : steps){
        bool improved = true;  // Flag to track if we found improvement

        // Keep searching at current step size until no improvement found
        while(improved){
            improved = false;  // Assume no improvement initially
            
            // Try moving in all 8 directions
            for(auto& dir : directions){
                // Calculate new position: current position + (direction × step)
                double newX = x + dir.first * step;
                double newY = y + dir.second * step;

                // Calculate total distance from new position
                double newDist = getTotalDistance(points, newX, newY);

                // If new position is better, move there
                if(newDist < minDist){
                    x = newX;
                    y = newY;
                    minDist = newDist;
                    improved = true;  // Found improvement, continue searching
                    
                    // Note: We don't break here - we check all directions
                    // to potentially find even better position in this iteration
                }
            }
            // If improved=true: while loop continues, try again from new position
            // If improved=false: no direction improved, done with this step size
        }
        // Move to next smaller step size for finer refinement
    }
    
    return {x, y};
}

/*
ALGORITHM VISUALIZATION:
Points: [(0,0), (0,10), (10,0), (10,10)]

Step 1: Initialize at centroid
  x = (0+0+10+10)/4 = 5.0
  y = (0+10+0+10)/4 = 5.0
  Initial position: (5.0, 5.0)
  Total distance ≈ 28.28

Step 2: Try step size 1.0
  Iteration 1: Try all 8 directions from (5.0, 5.0)
    - Check (4.0, 6.0), (5.0, 6.0), (6.0, 6.0), etc.
    - Suppose (5.0, 5.0) is already optimal at step=1.0
    - improved=false, exit while loop

Step 3: Try step size 0.1
  - Check positions like (4.9, 5.0), (5.1, 5.0), etc.
  - Make small adjustments if they reduce total distance
  - Continue until no improvement

Step 4: Try step sizes 0.01 and 0.001 for fine-tuning
  - Final result converges to geometric median

For these 4 points (corners of square), the geometric median is at center (5.0, 5.0)

WHY THIS WORKS:
- Centroid is good starting point (usually close to geometric median)
- Multi-resolution approach: coarse search first, then fine refinement
- Guaranteed to find local minimum (may not be global, but usually good enough)
- Much faster than brute force grid search
- No need to define search bounds

COMPARISON TO BRUTE FORCE:
- Brute Force: O(g² × n) where g can be 1000+ for fine grid → millions of evaluations
- Gradient Descent: O(k × 8 × n) where k ≈ 10-50 → hundreds of evaluations
- Speedup: 1000x-10000x faster!
*/

int main(){
    std::cout << "=== GEOMETRIC MEDIAN FINDER ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    
    // Test case 1: Corners of a square
    std::cout << "\n--- Test 1: Square corners ---" << std::endl;
    std::vector<std::vector<int>> points1 = {{0, 0}, {0, 10}, {10, 0}, {10, 10}};
    std::cout << "Points: (0,0), (0,10), (10,0), (10,10)" << std::endl;
    
    auto result1 = getMinDistancePoint(points1);
    std::cout << "Geometric median: (" << result1[0] << ", " << result1[1] << ")" << std::endl;
    std::cout << "Total distance: " << getTotalDistance(points1, result1[0], result1[1]) << std::endl;
    std::cout << "Expected: Near (5.0, 5.0) - center of square" << std::endl;
    
    // Compare with centroid
    double centroidX = 5.0, centroidY = 5.0;
    std::cout << "\nCentroid: (" << centroidX << ", " << centroidY << ")" << std::endl;
    std::cout << "Centroid total distance: " << getTotalDistance(points1, centroidX, centroidY) << std::endl;
    
    // Test case 2: Collinear points
    std::cout << "\n--- Test 2: Collinear points ---" << std::endl;
    std::vector<std::vector<int>> points2 = {{0, 0}, {5, 0}, {10, 0}};
    std::cout << "Points: (0,0), (5,0), (10,0)" << std::endl;
    
    auto result2 = getMinDistancePoint(points2);
    std::cout << "Geometric median: (" << result2[0] << ", " << result2[1] << ")" << std::endl;
    std::cout << "Total distance: " << getTotalDistance(points2, result2[0], result2[1]) << std::endl;
    std::cout << "Expected: (5.0, 0.0) - middle point" << std::endl;
    
    // Test case 3: Asymmetric distribution
    std::cout << "\n--- Test 3: Asymmetric distribution ---" << std::endl;
    std::vector<std::vector<int>> points3 = {{0, 0}, {1, 0}, {2, 0}, {10, 0}};
    std::cout << "Points: (0,0), (1,0), (2,0), (10,0)" << std::endl;
    
    auto result3 = getMinDistancePoint(points3);
    std::cout << "Geometric median: (" << result3[0] << ", " << result3[1] << ")" << std::endl;
    std::cout << "Total distance: " << getTotalDistance(points3, result3[0], result3[1]) << std::endl;
    std::cout << "Expected: Closer to (1.0-2.0, 0.0) - weighted toward cluster" << std::endl;
    
    return 0;
}
