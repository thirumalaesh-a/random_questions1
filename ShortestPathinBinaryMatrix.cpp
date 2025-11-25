#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <tuple>
#include <climits>  

int ShortestPathinBinaryMatrix(std::vector<std::vector<int>>& grid){
    int n = grid.size();
    if(grid[0][0] != 0 || grid[n-1][n-1] != 0) return -1;
    if(n == 1) return 1;

    // {f_score, row, col, g_score}
    using T = std::tuple<int, int, int, int>;  // Fixed: Tuple -> std::tuple
    std::priority_queue<T, std::vector<T>, std::greater<T>> pq;  // Fixed: greater -> std::greater
    
    auto heuristic = [&](int r, int c){
        return std::max(std::abs(r - (n-1)), std::abs(c - (n-1)));  // Fixed: max -> std::max, added missing comma
    };  // Fixed: Added missing semicolon
    
    int g_score = 1;
    int f_score = g_score + heuristic(0, 0);

    pq.push({f_score, 0, 0, g_score});

    std::vector<std::vector<int>> visited(n, std::vector<int>(n, INT_MAX));
    visited[0][0] = g_score;

    std::vector<std::pair<int, int>> dir = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
    
    while(!pq.empty()){
        auto [f, row, col, g] = pq.top();
        pq.pop();

        if(row == n-1 && col == n-1) return g;
        
        for(int i = 0; i < 8; i++){
            int newRow = row + dir[i].first;
            int newCol = col + dir[i].second;

            if(newRow >= 0 && newRow < n && newCol >= 0 && newCol < n && grid[newRow][newCol] == 0){
                int newG = g + 1;
                if(newG < visited[newRow][newCol]){
                    visited[newRow][newCol] = newG;
                    int newF = newG + heuristic(newRow, newCol);
                    pq.push({newF, newRow, newCol, newG});
                }
            } 
        }
    }  
    
    return -1;
}

int main(){
    // Fixed: Removed reference and extra braces
    std::vector<std::vector<int>> grid = {
        {0,0,0},{1,1,0},{1,1,0}
    };
    
    auto path = ShortestPathinBinaryMatrix(grid);
    std::cout << path << std::endl;
    return 0;
}