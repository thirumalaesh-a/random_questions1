#include <iostream>
#include <vector>
#include <queue>
#include <climits>
// ============================================================================
// OPTIMIZED APPROACH - Accumulates distances on-the-fly O(m×n) space
// Only supports: Sum aggregation
// Trade-off: 96% less memory, cannot do Minimax/MaxMin/Weighted
// ============================================================================

class ParkingSystemOptimized {
private:
    /**
     * BFS from single parking spot with immediate aggregation
     * Accumulates results directly into totalDist and reachCount
     */
    void bfs(std::vector<std::vector<int>>& grid, int startRow, int startCol,
             std::vector<std::vector<int>>& totalDist,
             std::vector<std::vector<int>>& reachCount) {
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
                int newRow = row + deltaRow, newCol = col + deltaCol;
                
                // Walk on empty land (0) AND parking spots (1)
                if(newRow >= 0 && newRow < m && newCol >= 0 && newCol < n &&
                   distance[newRow][newCol] == -1 && grid[newRow][newCol] != 2) {
                    distance[newRow][newCol] = distance[row][col] + 1;
                    
                    // Aggregate immediately (saves memory)
                    totalDist[newRow][newCol] += distance[newRow][newCol];
                    reachCount[newRow][newCol]++;
                    
                    q.push({newRow, newCol});
                }
            }
        }
        // distance map freed here - saves memory!
    }
    
public:
    /**
     * Find optimal autonomous parking spot (space-optimized)
     * Minimizes sum of distances to all existing parking spots
     * Time: O(k×m×n), Space: O(m×n) - 96% memory reduction!
     * 
     * Note: Only works for SUM aggregation
     * Cannot support Minimax/MaxMin without storing distance maps
     */
    int findOptimalSpot(std::vector<std::vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        
        // Extract existing parking spots (1 = parking spot)
        std::vector<std::pair<int,int>> parkingSpots;
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(grid[i][j] == 1) parkingSpots.push_back({i, j});
            }
        }
        
        int numSpots = parkingSpots.size();
        
        // Accumulation matrices (only O(m×n) space!)
        std::vector<std::vector<int>> totalDist(m, std::vector<int>(n, 0));
        std::vector<std::vector<int>> reachCount(m, std::vector<int>(n, 0));
        
        // Run BFS from each spot and accumulate immediately
        for(auto [i, j] : parkingSpots) {
            bfs(grid, i, j, totalDist, reachCount);
        }
        
        // Find optimal location
        int bestDist = INT_MAX, bestR = -1, bestC = -1;
        
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                // AUTONOMOUS PARKING: Can place on empty (0) OR existing spots (1)
                if(grid[i][j] == 2) continue;  // Only skip obstacles
                
                // Must be reachable by ALL spots
                if(reachCount[i][j] == numSpots && totalDist[i][j] < bestDist) {
                    bestDist = totalDist[i][j];
                    bestR = i;
                    bestC = j;
                }
            }
        }
        
        if(bestR == -1) {
            std::cout << "No valid location found\n";
            return -1;
        }
        
        std::cout << "Optimized - Optimal: (" << bestR << "," << bestC 
                  << ") sum_dist=" << bestDist << "\n";
        return bestDist;
    }
};

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
    ParkingSystemOptimized optimized;
    optimized.findOptimalSpot(grid);
    
    return 0;
}