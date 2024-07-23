#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>

class MazeGenerator {
public:
    MazeGenerator(int width, int height) : width(width), height(height) {
        maze_matrix.resize(height, std::vector<int>(width, 0));
    }

    void generate() {
        // Start from a random cell
        std::srand(std::time(0));
        int startX = std::rand() % width;
        int startY = std::rand() % height;

        // Initialize the maze with walls
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                maze_matrix[i][j] = 0;

        // Create a stack to hold the cells
        std::stack<std::pair<int, int>> cellStack;
        cellStack.push({ startX, startY });
        maze_matrix[startY][startX] = 1; // Mark the starting cell as a path

        while (!cellStack.empty()) {
            std::pair<int, int> current = cellStack.top();
            int x = current.first;
            int y = current.second;

            // Get the list of unvisited neighbors
            std::vector<std::pair<int, int>> neighbors;
            if (x > 1 && maze_matrix[y][x - 2] == 0)
                neighbors.push_back({ x - 2, y });
            if (x < width - 2 && maze_matrix[y][x + 2] == 0)
                neighbors.push_back({ x + 2, y });
            if (y > 1 && maze_matrix[y - 2][x] == 0)
                neighbors.push_back({ x, y - 2 });
            if (y < height - 2 && maze_matrix[y + 2][x] == 0)
                neighbors.push_back({ x, y + 2 });

            if (!neighbors.empty()) {
                // Choose a random neighbor
                int randIndex = std::rand() % neighbors.size();
                std::pair<int, int> next = neighbors[randIndex];

                // Remove the wall between the current cell and the chosen neighbor
                int newX = next.first;
                int newY = next.second;
                maze_matrix[(y + newY) / 2][(x + newX) / 2] = 1;
                maze_matrix[newY][newX] = 1;

                // Push the chosen neighbor to the stack
                cellStack.push(next);
            }
            else {
                // Backtrack
                cellStack.pop();
            }
        }
    }

    void printMaze() {
        for (const auto& row : maze_matrix) {
            for (int cell : row) {
                std::cout << (cell == 1 ? " " : "#");
            }
            std::cout << std::endl;
        }
    }

    const std::vector<std::vector<int>>& mazeMatrix() {
        return maze_matrix;
    }

private:
    int width, height;
    std::vector<std::vector<int>> maze_matrix;
};