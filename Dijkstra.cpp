#include <SDL.h>
#include <iostream>
#include <vector>

#include "ApplicationContext.hpp"
#include "Graph.hpp"

/*
* Grid code adapted from:
* https://github.com/catsocks/sdl-grid/
*/

int main(int argc , char* argv[]) {

	dijkstra::Application app{};

	app.initSDL();

	app.mainLoop();

	app.terminateSDL();

	return EXIT_SUCCESS;
}
