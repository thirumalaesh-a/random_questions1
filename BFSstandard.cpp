#include <iostream>
#include <vector>
#include <queue>
#include <climits>

// ============================================================================
// STANDARD APPROACH - Stores all distance maps O(k×m×n) space
// Supports: Sum, Minimax, MaxMin, Weighted aggregations
// ============================================================================

class ParkingSystemStandard {
private:
    /**
     * BFS from single parking spot
     * Returns: Distance map where [i][j] = distance from start to (i,j)
     */
    std::vector<std::vector<int>> bfs(std::vector<std::vector<int>>& grid, 
                                      int startRow, int startCol) {
        int m = grid.size(), n = grid[0].size();
        std::vector<std::vector<int>> distance(m, std::vector<int>(n, -1));
        std::queue<std::pair<int,int>> q;
        std::vector<std::pair<int,int>> directions = {{0,1},{1,0},{0,-1},{-1,0}};
        
        q.push({startRow, startCol});
        distance[startRow][startCol] = 0;
        
        while(!q.empty()) {
            auto [row, col] = q.front();
            q.pop();
            
            for(auto [deltaRow, deltaCol] : directions) {
                int newRow = row + deltaRow;
                int newCol = col + deltaCol;
                
                // Walk on empty land (0) AND parking spots (1)
                if(newRow >= 0 && newRow < m && newCol >= 0 && newCol < n &&
                   distance[newRow][newCol] == -1 && grid[newRow][newCol] != 2) {
                    distance[newRow][newCol] = distance[row][col] + 1;
                    q.push({newRow, newCol});
                }
            }
        }
        return distance;
    }
    
    /**
     * Print distance map for debugging
     */
    void printDistanceMap(std::vector<std::vector<int>>& distanceMap, int poiNum) {
        std::cout << "\nPOI " << poiNum << " distances:\n";
        for(int row = 0; row < distanceMap.size(); row++) {  
            for(int col = 0; col < distanceMap[row].size(); col++) {
                if(distanceMap[row][col] == -1) {
                    std::cout << "  X";
                } else {
                    std::cout << " " << distanceMap[row][col];
                }
            }
            std::cout << "\n";
        }
    }
    
public:
    /**
     * Find optimal autonomous parking spot
     * Minimizes sum of distances to all existing parking spots
     * Time: O(k×m×n), Space: O(k×m×n)
     * 
     * VARIANT 2 - MINIMAX: Minimize maximum distance (fairness)
     * Change line marked "SUM" to:
     *   maxDist = std::max(maxDist, allDistances[k][i][j]);
     * Use maxDist variable instead of totalDist
     * 
     * VARIANT 3 - MAXMIN: Maximize minimum distance (stay far)
     * Change line marked "SUM" to:
     *   minDist = std::min(minDist, allDistances[k][i][j]);
     * Change comparison to: minDist > bestDist
     */
    int findOptimalSpot(std::vector<std::vector<int>>& grid, bool debug = false) {
        int m = grid.size(), n = grid[0].size();
        
        // Extract existing parking spots (1 = parking spot)
        std::vector<std::pair<int,int>> parkingSpots;
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(grid[i][j] == 1) parkingSpots.push_back({i, j});
            }
        }
        
        int numSpots = parkingSpots.size();
        
        // Run BFS from each parking spot - store all distance maps
        std::vector<std::vector<std::vector<int>>> allDistances;
        int poiCount = 1;
        for(auto [i, j] : parkingSpots) {
            auto distMap = bfs(grid, i, j);
            if(debug) printDistanceMap(distMap, poiCount++);
            allDistances.push_back(distMap);
        }
        
        // Find optimal location
        int bestDist = INT_MAX, bestR = -1, bestC = -1;
        
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                // AUTONOMOUS PARKING: Can place on empty (0) OR existing spots (1)
                if(grid[i][j] == 2) continue;  // Only skip obstacles
                
                // Check reachability and aggregate distances
                int totalDist = 0, reach = 0;
                for(int k = 0; k < numSpots; k++) {
                    if(allDistances[k][i][j] == -1) break;  // Unreachable
                    reach++;
                    totalDist = allDistances[k][i][j];  // MiniMax aggregation
                }
                
                // Must be reachable by ALL spots
                if(reach == numSpots && totalDist < bestDist) {
                    bestDist = totalDist;
                    bestR = i;
                    bestC = j;
                }
            }
        }
        
        if(bestR == -1) {
            std::cout << "No valid location found\n";
            return -1;
        }
        
        std::cout << "Standard - Optimal: (" << bestR << "," << bestC 
                  << ") sum_dist=" << bestDist << "\n";
        return bestDist;
    }
};

// ============================================================================
// MAIN - Demonstrates both approaches
// ============================================================================

int main() {
    std::cout << "\n=== AUTONOMOUS PARKING SYSTEM ===\n\n";
    
    // Grid: 0 = empty land, 1 = parking spot, 2 = obstacle
    // Question: Where to place new autonomous parking spot?
    // Constraint: New spot can be on empty land OR existing parking spot
    std::vector<std::vector<int>> grid = {
        {1, 0, 2, 0, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0}
    };
    
    std::cout << "Grid (1=Parking Spot, 0=Empty, 2=Obstacle):\n";
    for (auto& row : grid) {
        for (int val : row) {
            if (val == 1) std::cout << "P ";
            else if (val == 2) std::cout << "X ";
            else std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    // Run both approaches
    ParkingSystemStandard standard;
    standard.findOptimalSpot(grid, true);  // debug=true to print distance maps
    
    
    return 0;
}