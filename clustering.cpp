/*
POINT CLUSTERING USING DISJOINT SET (UNION-FIND)

PROBLEM: Group 2D points into clusters based on distance threshold
- Points within distance k of each other belong to same cluster
- Use Union-Find to efficiently track connected components
- Return total number of distinct clusters

BRUTE FORCE APPROACH:
- For each point, run DFS/BFS to find all reachable points within distance k
- Mark visited points and count components
- Time Complexity: O(n²) for checking distances + O(n²) for traversal
- Space Complexity: O(n) for visited array

OPTIMIZED APPROACH (Current Implementation):
- Use Disjoint Set (Union-Find) with path compression and union by size
- Check all pairs once and union if distance <= k
- Count unique parents (cluster representatives)
- Time Complexity: O(n² × α(n)) where α is inverse Ackermann (nearly O(n²))
- Space Complexity: O(n) for parent and size arrays
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_set>

// ============================================================================
// DISJOINT SET (UNION-FIND) DATA STRUCTURE
// ============================================================================

class DisjointSet
{
private:
    std::vector<int> parent;  // parent[i] = parent of node i
    std::vector<int> size;    // size[i] = size of tree rooted at i

public:
    // Constructor: initialize n elements, each in its own set
    // Time: O(n)
    DisjointSet(int n){
        parent.resize(n);
        size.resize(n, 1);  // Each set initially has size 1
        
        // Initially, each node is its own parent (self-loop)
        for(int i = 0; i < n; i++){
            parent[i] = i;
        }
    }

    // Find ultimate parent (representative) of a node
    // Uses path compression: makes tree flatter for faster future queries
    // Time: O(α(n)) amortized, where α is inverse Ackermann function (nearly O(1))
    int findUpar(int node){
        // Base case: node is its own parent (root of tree)
        if (node == parent[node]){
            return node;
        }
        
        // Path compression: set parent directly to ultimate parent
        // This flattens the tree structure for faster future lookups
        // Example: If path is 1→2→3→4, after find(1), it becomes 1→4, 2→4, 3→4
        return parent[node] = findUpar(parent[node]);
    }

    // Union two sets by connecting their representatives
    // Uses union by size: attach smaller tree under larger tree
    // Time: O(α(n)) amortized
    void Unionfind(int u, int v){
        // Find ultimate parents of both nodes
        int ultimate_u = findUpar(u);
        int ultimate_v = findUpar(v);
        
        // Already in same set, nothing to do
        if(ultimate_u == ultimate_v) 
            return;
        
        // Union by size: attach smaller tree under root of larger tree
        // This keeps tree height small
        if(size[ultimate_u] < size[ultimate_v]){
            // Tree of u is smaller: make v's root the parent
            parent[ultimate_u] = ultimate_v;
            // Update size of v's tree
            size[ultimate_v] += size[ultimate_u];
        }
        else{
            // Tree of v is smaller or equal: make u's root the parent
            parent[ultimate_v] = ultimate_u;
            // Update size of u's tree
            size[ultimate_u] += size[ultimate_v];
        }
    }
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Calculate Euclidean distance between two 2D points
// Formula: sqrt((x2-x1)² + (y2-y1)²)
// Time: O(1)
double distance(std::pair<int, int>& a, std::pair<int, int>& b){
    int dx = a.first - b.first;    // Difference in x-coordinates
    int dy = a.second - b.second;  // Difference in y-coordinates
    
    // Euclidean distance formula
    return sqrt(dx * dx + dy * dy);
}

// ============================================================================
// MAIN CLUSTERING ALGORITHM
// ============================================================================

// Count number of clusters where points within distance k are connected
// Returns: number of distinct clusters
// Time: O(n² × α(n)) where n = number of points
int clustering(std::vector<std::pair<int, int>>& coords, double k){
    int n = coords.size();
    
    // Edge case: no points
    if(n == 0) 
        return -1;
    
    // Initialize disjoint set with n elements (one per point)
    DisjointSet ds(n);

    // Check all pairs of points
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            // If points are within distance k, they belong to same cluster
            if(distance(coords[i], coords[j]) <= k){
                // Union these two points (merge their clusters)
                ds.Unionfind(i, j);
            }
        }
    }

    // Count distinct clusters by counting unique parents
    std::unordered_set<int> uniqueParents;
    for(int i = 0; i < n; i++){
        // Find ultimate parent of each point
        // Points with same parent are in same cluster
        uniqueParents.insert(ds.findUpar(i));
    }
    
    // Number of unique parents = number of clusters
    return uniqueParents.size();
}

/*
DETAILED EXAMPLE WALKTHROUGH:
Input: coords = [(0,0), (0,1), (5,0), (5,1)], k = 1.5

Step 1: Initialize DisjointSet
  parent = [0, 1, 2, 3]  (each point is its own parent)
  size = [1, 1, 1, 1]

Step 2: Check all pairs and union if distance <= k
  Pair (0,1): distance = sqrt((0-0)² + (0-1)²) = 1.0 <= 1.5
    → Union(0, 1): parent = [1, 1, 2, 3], size = [1, 2, 1, 1]
  
  Pair (0,2): distance = sqrt((0-5)² + (0-0)²) = 5.0 > 1.5
    → No union
  
  Pair (0,3): distance = sqrt((0-5)² + (0-1)²) ≈ 5.1 > 1.5
    → No union
  
  Pair (1,2): distance = sqrt((0-5)² + (1-0)²) ≈ 5.1 > 1.5
    → No union
  
  Pair (1,3): distance = sqrt((0-5)² + (1-1)²) = 5.0 > 1.5
    → No union
  
  Pair (2,3): distance = sqrt((5-5)² + (0-1)²) = 1.0 <= 1.5
    → Union(2, 3): parent = [1, 1, 3, 3], size = [1, 2, 1, 2]

Step 3: Count unique parents
  findUpar(0) = 1  (points 0 and 1 in same cluster)
  findUpar(1) = 1
  findUpar(2) = 3  (points 2 and 3 in same cluster)
  findUpar(3) = 3
  
  uniqueParents = {1, 3}
  
Result: 2 clusters
  Cluster 1: {(0,0), (0,1)}
  Cluster 2: {(5,0), (5,1)}

WHY UNION-FIND?
- Efficiently handles transitive connections: if A connects to B and B connects to C, 
  then A, B, C are all in same cluster
- Path compression and union by size make operations nearly O(1)
- Better than DFS/BFS for multiple connectivity queries
*/

int main(){
    std::cout << "=== POINT CLUSTERING EXAMPLE ===" << std::endl;
    
    // Define test points: two pairs of close points far apart from each other
    std::vector<std::pair<int, int>> coords = {
        {0, 0},   // Point 0
        {0, 1},   // Point 1 (close to point 0)
        {5, 0},   // Point 2 (far from points 0,1)
        {5, 1}    // Point 3 (close to point 2)
    };
    
    // Distance threshold for clustering
    double k = 1.5;
    
    std::cout << "Points: ";
    for(const auto& p : coords){
        std::cout << "(" << p.first << "," << p.second << ") ";
    }
    std::cout << std::endl;
    std::cout << "Distance threshold k = " << k << std::endl;
    
    // Run clustering algorithm
    int groups = clustering(coords, k);
    
    std::cout << "\nNumber of clusters: " << groups << " (expected: 2)" << std::endl;
    std::cout << "Cluster 1: (0,0) and (0,1)" << std::endl;
    std::cout << "Cluster 2: (5,0) and (5,1)" << std::endl;
    
    // Additional test: smaller threshold
    std::cout << "\n=== TEST WITH SMALLER THRESHOLD ===" << std::endl;
    double k2 = 0.5;
    int groups2 = clustering(coords, k2);
    std::cout << "Distance threshold k = " << k2 << std::endl;
    std::cout << "Number of clusters: " << groups2 << " (expected: 4, all separate)" << std::endl;
    
    return 0;
}