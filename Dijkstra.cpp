#include <SDL.h>
#include <iostream>
#include <vector>

#include "ApplicationContext.hpp"

/*
* Grid code adapted from:
* https://github.com/catsocks/sdl-grid/
*/

const int k_maze_width = 100;
const int k_maze_height = 50;

int main(int argc , char* argv[]) {

	dijkstra::Application app{k_maze_width,k_maze_height,10};

	app.initSDL();

	app.mainLoop();

	app.terminateSDL();

	return EXIT_SUCCESS;
}
