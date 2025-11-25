/*
ROTATE MATRIX GRID LAYERS COUNTER-CLOCKWISE

PROBLEM: Rotate each layer of a matrix counter-clockwise by k positions
- Matrix has concentric rectangular layers (like onion rings)
- Each layer rotates independently
- Elements move counter-clockwise around their layer
- Example: 4x4 matrix has 2 layers (outer border and inner 2x2)

VISUALIZATION:
Original 4x4 matrix:          Layer 0 (outer):        Layer 1 (inner):
 1  2  3  4                    1  2  3  4              . . . .
 5  6  7  8                    5  .  .  8              . 6  7 .
 9 10 11 12                    9  . . 12               .10 11 .
13 14 15 16                   13 14 15 16              . . . .

After rotating layer 0 by k=2 counter-clockwise:
- Element at position 0 moves to position 2
- Element at position 1 moves to position 3
- etc.

BRUTE FORCE APPROACH:
- For each rotation step (1 to k):
  - Extract all elements of layer
  - Shift each element one position counter-clockwise
  - Put elements back
- Time Complexity: O(layers × k × perimeter) = O(m×n×k) worst case
- Space Complexity: O(1)
- Problem: Very slow for large k

OPTIMIZED APPROACH (Current Implementation):
- Extract layer elements into array once
- Use modulo to calculate final position after k rotations
- Place elements back in rotated positions
- Time Complexity: O(layers × perimeter) = O(m×n)
- Space Complexity: O(max(m,n)) for temporary storage per layer
- Advantage: k rotations done in single pass, k can be huge

KEY INSIGHT: Rotating k times is same as shifting array by k positions
- Instead of rotating k times, just place each element at (original_pos + k) % perimeter
*/

#include <iostream>
#include <vector>

// ============================================================================
// ROTATE SINGLE LAYER OF MATRIX
// ============================================================================

// Rotate a single rectangular layer counter-clockwise by k positions
// Layer: concentric rectangle at given depth from outer border
// Time: O(perimeter of layer) = O(m+n)
// Space: O(perimeter) for temporary storage
void rotateLayer(std::vector<std::vector<int>>& grid, int layer, int k) {
    int m = grid.size();       // Number of rows
    int n = grid[0].size();    // Number of columns
    
    // Calculate boundaries of current layer
    int top = layer;           // Top row index
    int bottom = m - 1 - layer; // Bottom row index
    int left = layer;          // Left column index
    int right = n - 1 - layer; // Right column index
    
    // ========================================================================
    // STEP 1: Calculate perimeter and normalize k
    // ========================================================================
    // Perimeter = 2 × (width + height)
    // Example: 4x4 outer layer has perimeter = 2×(4+4) = 16
    int perimeter = 2 * (right - left + bottom - top);
    
    // Optimize k: rotating by perimeter brings back to same position
    // So k=17 is same as k=1 for perimeter=16
    k = k % perimeter;
    
    // If k is multiple of perimeter, no rotation needed
    if (k == 0) return;
    
    // ========================================================================
    // STEP 2: Extract layer elements into temporary array (clockwise order)
    // ========================================================================
    std::vector<int> temp;
    
    // Traverse layer in clockwise order and extract elements
    // This creates a 1D representation of the layer border
    
    // Top row: left to right
    for (int j = left; j <= right; j++) {
        temp.push_back(grid[top][j]);
    }
    
    // Right column: top+1 to bottom (skip top corner, already added)
    for (int i = top + 1; i <= bottom; i++) {
        temp.push_back(grid[i][right]);
    }
    
    // Bottom row: right-1 to left (if exists, skip single-row layer)
    if (bottom > top) {
        for (int j = right - 1; j >= left; j--) {
            temp.push_back(grid[bottom][j]);
        }
    }
    
    // Left column: bottom-1 to top+1 (if exists, skip single-column layer)
    if (right > left) {
        for (int i = bottom - 1; i > top; i--) {
            grid[i][left];
        }
    }
    
    // ========================================================================
    // STEP 3: Place elements back in rotated positions
    // ========================================================================
    // Counter-clockwise rotation by k means: element at position i goes to position (i+k)%perimeter
    // We start reading from position k in temp array
    
    int start = k; // Start index in temp array (implements rotation)
    
    // Top row: place rotated elements
    for (int j = left; j <= right; j++) {
        // Get element from rotated position in temp array
        grid[top][j] = temp[start++ % temp.size()];
    }
    
    // Right column: place rotated elements
    for (int i = top + 1; i <= bottom; i++) {
        grid[i][right] = temp[start++ % temp.size()];
    }
    
    // Bottom row: place rotated elements (if exists)
    if (bottom > top) {
        for (int j = right - 1; j >= left; j--) {
            grid[bottom][j] = temp[start++ % temp.size()];
        }
    }
    
    // Left column: place rotated elements (if exists)
    if (right > left) {
        for (int i = bottom - 1; i > top; i--) {
            grid[i][left] = temp[start++ % temp.size()];
        }
    }
}

// ============================================================================
// ROTATE ALL LAYERS OF MATRIX
// ============================================================================

// Rotate entire matrix by rotating each layer independently
// Returns: modified grid with all layers rotated
// Time: O(m×n) - visit each element constant number of times
// Space: O(max(m,n)) - temporary storage for largest layer
std::vector<std::vector<int>> rotateGrid(std::vector<std::vector<int>>& grid, int k) {
    int m = grid.size();       // Number of rows
    int n = grid[0].size();    // Number of columns
    
    // Calculate number of layers (concentric rectangles)
    // A 4x4 matrix has 2 layers, a 5x5 matrix has 2 layers (with 1x1 center)
    int layers = std::min(m, n) / 2;
    
    // Rotate each layer independently
    // Layer 0 = outermost, layer 1 = next inner, etc.
    for (int layer = 0; layer < layers; layer++) {
        rotateLayer(grid, layer, k);
    }
    
    return grid;
}

/*
DETAILED EXAMPLE WALKTHROUGH:
Original 4x4 matrix, k=2:
 1  2  3  4
 5  6  7  8
 9 10 11 12
13 14 15 16

LAYER 0 (outer border):
Elements in clockwise order: [1,2,3,4,8,12,16,15,14,13,9,5]
Perimeter = 12
k=2, so rotate counter-clockwise by 2 positions

After rotation by 2:
Start reading from index 2: [3,4,8,12,16,15,14,13,9,5,1,2]

Place back:
- Top row: 3,4,8,12 → but we only need 4 elements: 3,4,8,12? NO!
Let me recalculate:

Actually, temp = [1,2,3,4, 8,12,16, 15,14,13, 9,5]
             indices: 0,1,2,3, 4,5,6,   7,8,9,   10,11

Counter-clockwise by k=2 means element at index i goes to position (i-k)
Or equivalently, position i gets element from index (i+k) % perimeter

So starting from index k=2:
Position 0 gets temp[2] = 3
Position 1 gets temp[3] = 4
Position 2 gets temp[4] = 8
...
Position 10 gets temp[0] = 1
Position 11 gets temp[1] = 2

Result after layer 0:
 3  4  8 12
 5  6  7 16
 9 10 11 15
 1  2 14 13

LAYER 1 (inner 2x2):
Elements: [6,7,11,10]
Perimeter = 4
k=2

After rotation by 2:
temp[2,3,0,1] = [11,10,6,7]

Result after layer 1:
 3  4  8 12
 5 11 10 16
 9  6  7 15
 1  2 14 13

WHY THIS APPROACH?
- Single pass per layer (O(perimeter)) vs k passes (O(k×perimeter))
- Works efficiently even if k is very large (k=1000000)
- Uses modulo arithmetic to avoid actual rotations
*/

int main() {
    std::cout << "=== MATRIX LAYER ROTATION ===" << std::endl;
    
    // Initialize 4x4 matrix
    std::vector<std::vector<int>> grid = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    
    std::cout << "Original matrix:" << std::endl;
    for (const auto& row : grid) {
        for (int val : row) {
            std::cout << std::setw(3) << val << " ";
        }
        std::cout << std::endl;
    }
    
    // Rotate by k=2 counter-clockwise
    int k = 2;
    std::cout << "\nRotating by k=" << k << " counter-clockwise..." << std::endl;
    
    rotateGrid(grid, k);
    
    std::cout << "\nAfter rotation:" << std::endl;
    for (const auto& row : grid) {
        for (int val : row) {
            std::cout << std::setw(3) << val << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}