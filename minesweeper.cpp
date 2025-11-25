/*
MINESWEEPER GAME IMPLEMENTATION

PROBLEM: Reveal cells on a minesweeper board after a click
- If click on mine ('M'): mark as 'X' and game over
- If click on empty ('E'): reveal adjacent mine count or expand blank region
- Blank cells ('B') trigger recursive reveal of neighbors

BRUTE FORCE APPROACH:
- On each click, scan all 8 neighbors to count adjacent mines
- Use DFS/BFS to reveal connected blank regions
- Time Complexity: O(m*n) per click for mine counting + O(m*n) for DFS
- Space Complexity: O(m*n) for recursion stack in worst case

OPTIMIZED APPROACH (Current Implementation):
- Pre-compute mine counts for all cells once: O(m*n)
- Reuse pre-computed counts for all subsequent clicks
- DFS reveal only visits each cell once across all clicks
- Time Complexity: O(m*n) one-time preprocessing + O(k) per click (k = revealed cells)
- Space Complexity: O(m*n) for mine count matrix + O(m*n) recursion stack worst case
*/

#include <vector>
#include <iostream>

class Minesweeper
{
private:
    // 8 directions: top-left, top, top-right, left, right, bottom-left, bottom, bottom-right
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1},
    };
    
    // Pre-computed mine counts for each cell (optimization)
    std::vector<std::vector<int>> mineCounts;
    
    // Pre-compute adjacent mine counts for all cells on the board
    // Time: O(m*n*8) = O(m*n), Space: O(m*n)
    void preComputeMines(std::vector<std::vector<char>>& board, int rows, int cols)
    {
        mineCounts.assign(rows, std::vector<int>(cols, 0));
        
        // Iterate through every cell
        for(int r = 0; r < rows; ++r){
            for(int c = 0; c < cols; ++c){
                // If current cell is a mine
                if(board[r][c] == 'M'){
                    // Increment mine count for all 8 neighbors
                    for(const auto& dir : directions){
                        int newRow = r + dir.first;
                        int newCol = c + dir.second;
                        
                        // Check bounds and ensure neighbor isn't a mine
                        if(newRow >= 0 && newRow < rows && 
                           newCol >= 0 && newCol < cols && 
                           board[newRow][newCol] != 'M')
                        {
                            ++mineCounts[newRow][newCol];
                        } 
                    }
                }
            }
        }
    }

    // DFS to reveal cells starting from clicked position
    // Time: O(k) where k = number of cells revealed, Space: O(k) recursion depth
    void dfsReveal(std::vector<std::vector<char>>& board, int rows, int cols, int clickRow, int clickCol)
    {
        // Base cases: out of bounds or cell already revealed/mine
        if(clickRow < 0 || clickRow >= rows || 
           clickCol < 0 || clickCol >= cols || 
           board[clickRow][clickCol] != 'E')
        {
            return;
        }
        
        // Get pre-computed mine count for this cell
        int mines = mineCounts[clickRow][clickCol];

        // If adjacent mines exist, reveal the count
        if(mines > 0)
        {
            board[clickRow][clickCol] = '0' + mines;
        }
        else{
            // No adjacent mines - mark as blank and recursively reveal neighbors
            board[clickRow][clickCol] = 'B';
            
            // Explore all 8 directions
            for(const auto& dir : directions)
            {
                int newClickRow = clickRow + dir.first;
                int newClickCol = clickCol + dir.second;
                dfsReveal(board, rows, cols, newClickRow, newClickCol);
            }
        }
    }

public:
    // Utility function to print the board state
    void printBoard(std::vector<std::vector<char>>& board)
    {
        for(int r = 0; r < board.size(); ++r)
        {
            for(int c = 0; c < board[r].size(); ++c)
            {
                std::cout << board[r][c] << " ";
            }
            std::cout << std::endl;
        }
    }
    
    // Main function to update board after a click
    // Time: O(m*n) first call + O(k) subsequent calls, Space: O(m*n)
    std::vector<std::vector<char>> updateBoard(std::vector<std::vector<char>>& board, std::vector<int>& click)
    {
        int rows = board.size();
        if(rows == 0) return board;
        int cols = board[0].size();

        int clickRow = click[0];
        int clickCol = click[1];

        // Validate click position
        if(clickRow < 0 || clickRow >= rows || clickCol < 0 || clickCol >= cols)
        {
            return board;
        }

        // Pre-compute mine counts on first click (lazy initialization)
        if(mineCounts.empty())
        {
            preComputeMines(board, rows, cols);
        }

        // If clicked on a mine, mark as 'X' and game over
        if(board[clickRow][clickCol] == 'M'){
            board[clickRow][clickCol] = 'X';
            return board;
        }

        // Reveal cells using DFS
        dfsReveal(board, rows, cols, clickRow, clickCol);
        return board;
    }
};

int main()
{
    Minesweeper game;
    
    // Initialize game board: 'E' = empty, 'M' = mine
    std::vector<std::vector<char>> board = {
        {'E', 'E', 'E', 'E', 'E'},
        {'M', 'E', 'M', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E'}
    };

    // Simulate a click at position (3, 4)
    std::vector<int> click = {3, 4};
    game.updateBoard(board, click);
    game.printBoard(board);
    
    return 0;
}