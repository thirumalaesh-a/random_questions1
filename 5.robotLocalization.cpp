/*
PROBLEM: Find robots in a 2D plane using func(lat, lon, radius) interface

APPROACHES:
1. Brute Force: Try every point in the grid at PRECISION intervals
   Time: O(N * A/P²) where A = area, P = precision, N = num robots
   Space: O(K) where K = number of robots found
   
2. Divide and Conquer (This approach):
   Time: O(N * log(A/P²)) = O(N * log(A))
   Space: O(log(A)) for recursion stack + O(K) for results
   
   Explanation:
   - Each level divides area by 4 (quadtree)
   - Max depth: log₄(A/P²) = O(log A)
   - At each node, we call func() once: O(N) to check all robots
   - Total: O(N * depth) = O(N * log A)
*/

#include <vector>
#include <iostream>
#include <cmath>
#include <climits>
#include <algorithm>

// Robot database (simulating external system)
static std::vector<std::pair<double, double>> robots = {
    {0.0, 0.0},
    {100.0, 50.0},
    {-50.0, -30.0},
    {75.0, 75.0},
    {-100.0, 100.0}
};

// External interface: check if robot exists within radius of given location
bool func(double lat, double lon, double radius) {
    for (const auto& robot : robots) {
        double dx = robot.first - lat;
        double dy = robot.second - lon;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= radius) {
            return true;
        }
    }
    return false;
}

const double PRECISION = 1e-6;
const double LAT_MIN = -180.0;
const double LON_MIN = -180.0;
const double LAT_MAX = 180.0;
const double LON_MAX = 180.0;

// ==================== SQUARE REGION SEARCH ====================
void searchSquare(double lat, double lon, double size, 
                  std::vector<std::pair<double, double>>& result, int K) {
    // Base cases
    if (size <= 0 || lat < LAT_MIN || lon < LON_MIN || 
        lat + size > LAT_MAX || lon + size > LON_MAX || (int)result.size() >= K) {
        return;
    }
    
    // Center of the square
    double centerLat = lat + size / 2.0;
    double centerLon = lon + size / 2.0;
    
    // Radius to cover entire square: diagonal/2 = sqrt(2)*size/2
    double radius = std::sqrt(2.0) * size / 2.0;
    
    // Early termination: no robot in this region
    if (!func(centerLat, centerLon, radius)) {
        return;
    }
    
    // Found robot at sufficient precision
    if (size <= PRECISION) {
        result.push_back({centerLat, centerLon});
        return;
    }
    
    // Divide into 4 quadrants and search recursively
    double halfSize = size / 2.0;
    searchSquare(lat, lon, halfSize, result, K);                      // Bottom-left
    searchSquare(lat + halfSize, lon, halfSize, result, K);           // Top-left
    searchSquare(lat, lon + halfSize, halfSize, result, K);           // Bottom-right
    searchSquare(lat + halfSize, lon + halfSize, halfSize, result, K); // Top-right
}

// ==================== RECTANGLE REGION SEARCH ====================
void searchRectangle(double lat, double lon, double latSize, double lonSize,
                     std::vector<std::pair<double, double>>& result, int K) {
    // Base cases
    if (latSize <= 0 || lonSize <= 0 || lat < LAT_MIN || lon < LON_MIN || 
        lat + latSize > LAT_MAX || lon + lonSize > LON_MAX || (int)result.size() >= K) {
        return;
    }
    
    // Center of the rectangle
    double centerLat = lat + latSize / 2.0;
    double centerLon = lon + lonSize / 2.0;
    
    // FOR RECTANGLE: Radius = sqrt(latSize² + lonSize²) / 2
    // This is the diagonal distance from center to corner
    double radius = std::sqrt(latSize * latSize + lonSize * lonSize) / 2.0;
    
    // Early termination: no robot in this region
    if (!func(centerLat, centerLon, radius)) {
        return;
    }
    
    // Found robot at sufficient precision
    if (latSize <= PRECISION && lonSize <= PRECISION) {
        result.push_back({centerLat, centerLon});
        return;
    }
    
    // OPTIMIZATION: Split along longer dimension first for better pruning
    if (latSize > lonSize) {
        double halfLat = latSize / 2.0;
        searchRectangle(lat, lon, halfLat, lonSize, result, K);
        searchRectangle(lat + halfLat, lon, halfLat, lonSize, result, K);
    } else {
        double halfLon = lonSize / 2.0;
        searchRectangle(lat, lon, latSize, halfLon, result, K);
        searchRectangle(lat, lon + halfLon, latSize, halfLon, result, K);
    }
    
    // ALTERNATIVE: Always split both dimensions (like square)
    // double halfLat = latSize / 2.0;
    // double halfLon = lonSize / 2.0;
    // searchRectangle(lat, lon, halfLat, halfLon, result, K);
    // searchRectangle(lat + halfLat, lon, halfLat, halfLon, result, K);
    // searchRectangle(lat, lon + halfLon, halfLat, halfLon, result, K);
    // searchRectangle(lat + halfLat, lon + halfLon, halfLat, halfLon, result, K);
}

// ==================== PUBLIC API ====================
std::vector<std::pair<double, double>> findRobotsSquare(int K = INT_MAX) {
    std::vector<std::pair<double, double>> result;
    double size = std::max(LAT_MAX - LAT_MIN, LON_MAX - LON_MIN);
    searchSquare(LAT_MIN, LON_MIN, size, result, K);
    return result;
}

std::vector<std::pair<double, double>> findRobotsRectangle(int K = INT_MAX) {
    std::vector<std::pair<double, double>> result;
    double latSize = LAT_MAX - LAT_MIN;
    double lonSize = LON_MAX - LON_MIN;
    searchRectangle(LAT_MIN, LON_MIN, latSize, lonSize, result, K);
    return result;
}

int main() {
    // ==================== TEST: func() interface ====================
    std::cout << "==================== TESTING func() INTERFACE ====================" << std::endl;
    std::cout << "Robot within radius 10 of (0,0): " << (func(0, 0, 10) ? "YES" : "NO") << std::endl;
    std::cout << "Robot within radius 5 of (0,0): " << (func(0, 0, 5) ? "YES" : "NO") << std::endl;
    std::cout << "Robot within radius 100 of (100,50): " << (func(100, 50, 100) ? "YES" : "NO") << std::endl;
    
    // ==================== SQUARE SEARCH: Find single robot ====================
    std::cout << "\n==================== SQUARE SEARCH: SINGLE ROBOT ====================" << std::endl;
    std::vector<std::pair<double, double>> singleResult = findRobotsSquare(1);
    if (singleResult.empty()) {
        std::cout << "No Robot Found" << std::endl;
    } else {
        std::cout << "Found robot at: (" << singleResult[0].first 
             << ", " << singleResult[0].second << ")" << std::endl;
    }
    
    // ==================== SQUARE SEARCH: Find all robots ====================
    std::cout << "\n==================== SQUARE SEARCH: ALL ROBOTS ====================" << std::endl;
    std::vector<std::pair<double, double>> allSquareResults = findRobotsSquare();
    std::cout << "Total robots found: " << allSquareResults.size() << std::endl;
    for (size_t i = 0; i < allSquareResults.size(); i++) {
        std::cout << "  Robot " << i+1 << ": (" 
             << allSquareResults[i].first << ", " << allSquareResults[i].second << ")" << std::endl;
    }
    
    // ==================== RECTANGLE SEARCH: Find all robots ====================
    std::cout << "\n==================== RECTANGLE SEARCH: ALL ROBOTS ====================" << std::endl;
    std::vector<std::pair<double, double>> allRectResults = findRobotsRectangle();
    std::cout << "Total robots found: " << allRectResults.size() << std::endl;
    for (size_t i = 0; i < allRectResults.size(); i++) {
        std::cout << "  Robot " << i+1 << ": (" 
             << allRectResults[i].first << ", " << allRectResults[i].second << ")" << std::endl;
    }
    
    // ==================== EDGE CASES ====================
    std::cout << "\n==================== EDGE CASES ====================" << std::endl;
    
    // Test with no robots
    robots.clear();
    auto noRobots = findRobotsSquare();
    std::cout << "Empty space test: Found " << noRobots.size() << " robots (expected 0)" << std::endl;
    
    // Test with boundary robots
    robots = {{LAT_MIN, LON_MIN}, {LAT_MAX - 0.1, LON_MAX - 0.1}};
    auto boundaryRobots = findRobotsSquare();
    std::cout << "Boundary robots: Found " << boundaryRobots.size() << " robots (expected 2)" << std::endl;
    
    // Test with single robot
    robots = {{50.0, 50.0}};
    auto singleRobot = findRobotsSquare();
    std::cout << "Single robot: Found " << singleRobot.size() << " robots (expected 1)" << std::endl;
    
    // ==================== COMPLEXITY ANALYSIS ====================
    std::cout << "\n==================== COMPLEXITY ANALYSIS ====================" << std::endl;
    std::cout << "Brute Force:" << std::endl;
    std::cout << "  Time: O(N * A/P²) where A=area, P=precision, N=#robots" << std::endl;
    std::cout << "  Space: O(K) where K=#robots found" << std::endl;
    std::cout << "\nDivide & Conquer (Current):" << std::endl;
    std::cout << "  Time: O(N * log(A))" << std::endl;
    std::cout << "  Space: O(log(A)) recursion + O(K) results" << std::endl;
    std::cout << "\nWhy it's better:" << std::endl;
    std::cout << "  - Prunes empty regions immediately" << std::endl;
    std::cout << "  - Logarithmic depth vs linear scan" << std::endl;
    std::cout << "  - Early termination when K robots found" << std::endl;
    
    // ==================== FOLLOW-UP: Moving Robots ====================
    std::cout << "\n==================== FOLLOW-UP: MOVING ROBOTS ====================" << std::endl;
    std::cout << "If robots can move:" << std::endl;
    std::cout << "  1. Expand search radius: r' = r + v_max * t" << std::endl;
    std::cout << "  2. Add timestamps to queries" << std::endl;
    std::cout << "  3. Re-verify locations before returning" << std::endl;
    std::cout << "  4. Use prediction models if movement is predictable" << std::endl;
    
    std::cout << "\n==================== FOLLOW-UP: SCALABILITY ====================" << std::endl;
    std::cout << "For large scale:" << std::endl;
    std::cout << "  1. Parallelize quadrant searches (independent)" << std::endl;
    std::cout << "  2. Use spatial indexing (R-tree, quadtree) for func()" << std::endl;
    std::cout << "  3. Adaptive precision based on robot density" << std::endl;
    std::cout << "  4. Caching func() results for overlapping queries" << std::endl;
    std::cout << "  5. Batch queries to reduce API calls" << std::endl;
    
    return 0;
}
