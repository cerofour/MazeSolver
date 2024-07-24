#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <limits>

// ObstacleGenerator class to generate a grid with obstacles
class ObstacleGenerator {
public:
    ObstacleGenerator(int width, int height, int obstaclePercentage)
        : width(width), height(height), obstaclePercentage(obstaclePercentage) {
        maze.resize(height, std::vector<int>(width, 0));
    }

    void generate() {
        std::srand(std::time(0));

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                maze[i][j] = (std::rand() % 100 < obstaclePercentage) ? 1 : 0;
            }
        }
    }

    void printMaze() {
        for (const auto& row : maze) {
            for (int cell : row) {
                std::cout << (cell == 1 ? "#" : ".");
            }
            std::cout << std::endl;
        }
    }

    const std::vector<std::vector<int>>& matrix() const {
        return maze;
    }

private:
    int width, height, obstaclePercentage;
    std::vector<std::vector<int>> maze;
};
