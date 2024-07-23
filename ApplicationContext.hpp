#include <SDL.h>
#include <tuple>
#include <memory>
#include <iostream>

#include "Graph.hpp"

namespace dijkstra {

	/*
	* App configuration
	*/

	const int k_grid_cell_size = 64;
	const int k_grid_width = 3;
	const int k_grid_height = 1;
	constexpr int k_window_width = (k_grid_width * k_grid_cell_size) + 1;
	constexpr int k_window_height = (k_grid_height * k_grid_cell_size) + 1;

	/*
	* Do the reverse calculation
	*/
	//const int window_width = 800;
	//const int window_height = 600;

	//constexpr int grid_width = (window_width - 1) / grid_cell_size;
	//constexpr int grid_height = (window_height - 1) / grid_cell_size;

	class Application {
	public:
		Application() {
			init();
		}

	public:
		bool initSDL() {
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION , "Initialize SDL: %s" ,
					SDL_GetError());
				return false;
			}
			if (SDL_CreateWindowAndRenderer(window_width , window_height , 0 , &window ,
				&renderer) < 0) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION ,
					"Create window and renderer: %s" , SDL_GetError());
				return false;
			}
			SDL_SetWindowTitle(window , "SDL Grid");
			return true;
		}

		void terminateSDL() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void mainLoop() {
			while (!quit) {
				SDL_Event event;
				while (SDL_PollEvent(&event)) {
					handleSDLEvents(event);
				}

				drawBackground();

				drawGrid();

				//drawCursors();

				drawNodes();

				if (visualize_dijkstra)
					drawDijkstra();

				drawGhostCursor();

				SDL_RenderPresent(renderer);
			}
		}

	public:
		int windowWidth() {
			return window_width;
		}

		int windowHeight() {
			return window_height;
		}

		int gridWidth() {
			return grid_width;
		}

		int gridHeight() {
			return grid_height;
		}

		std::tuple<int , int> getSelectedNode() {
			return std::make_pair(grid_cursor.x / grid_cell_size , grid_cursor.y / grid_cell_size);
		}

	private:
		void init() {
			grid_cell_size = k_grid_cell_size;
			grid_width = k_grid_width;
			grid_height = k_grid_height;
			window_width = k_window_width;
			window_height = k_window_height;

			/*
			* Each rectangle in the grid is indexed by two X and Y coordinates:
			* The exact position of a triangle is (X*grid_cell_size, Y*grid_cell_size)
			*/

			// Place the grid cursor in the middle of the screen.
			grid_cursor = {
				.x = (grid_width - 1) / 2 * grid_cell_size,
				.y = (grid_height - 1) / 2 * grid_cell_size,
				.w = grid_cell_size,
				.h = grid_cell_size,
			};

			// The cursor ghost is a cursor that always shows in the cell below the
			// mouse cursor.
			grid_cursor_ghost = grid_cursor;

			// Dark theme.
			grid_background = { 22, 22, 22, 255 }; // Barely Black
			grid_line_color = { 44, 44, 44, 255 }; // Dark grey
			grid_cursor_ghost_color = { 44, 44, 44, 255 };
			grid_cursor_color = { 255, 255, 255, 255 }; // White

			// Light Theme.
			// grid_background = {233, 233, 233, 255}; // Barely white
			// grid_line_color = {200, 200, 200, 255}; // Very light grey
			// grid_cursor_ghost_color = {200, 200, 200, 255};
			// grid_cursor_color = {160, 160, 160, 255}; // Grey

			starting_node_color = { 252, 186, 3, 255 };
			target_node_color = { 252, 20, 45, 255 };

			// this line is fucking stupid
			graph = std::make_unique<dijkstra::WeightedGraph>(dijkstra::WeightedGraph::createAdjacencyList(grid_width, grid_height));

			index_to_coords_map = std::make_unique<std::vector<std::tuple<int, int>>>();

			for (int x = 0; x < grid_height; x++)
				for (int y = 0; y < grid_width; y++)
					index_to_coords_map->push_back(std::make_pair(y, x));
		}

		void drawDijkstra() {
			for (const int i : *dijkstra_solution) {
				auto [x, y] = index_to_coords_map->at(i);
				SDL_Rect cell = {
					.x = x * grid_cell_size,
					.y = y * grid_cell_size,
					.w = grid_cell_size,
					.h = grid_cell_size,
				};
				SDL_SetRenderDrawColor(renderer, 230, 80, 14, 255);
				SDL_RenderFillRect(renderer, &cell);
			}
		}

		void drawBackground() {
			// Draw grid background.
			SDL_SetRenderDrawColor(renderer , grid_background.r , grid_background.g ,
				grid_background.b , grid_background.a);
			SDL_RenderClear(renderer);
		}

		void drawGrid() {
			// Draw grid lines.
			SDL_SetRenderDrawColor(renderer , grid_line_color.r , grid_line_color.g ,
				grid_line_color.b , grid_line_color.a);
			for (int x = 0; x < window_width; x += grid_cell_size) {
				SDL_RenderDrawLine(renderer , x , 0 , x , window_height);
			}
			for (int y = 0; y < window_height; y += grid_cell_size) {
				SDL_RenderDrawLine(renderer , 0 , y , window_width , y);
			}
		}

		void drawCursors() {
			// Draw grid cursor.
			SDL_SetRenderDrawColor(renderer , grid_cursor_color.r ,
				grid_cursor_color.g , grid_cursor_color.b ,
				grid_cursor_color.a);
			SDL_RenderFillRect(renderer , &grid_cursor);
		}

		void drawGhostCursor() {
			// Draw grid ghost cursor.
			if (mouse_active && mouse_hover) {
				SDL_SetRenderDrawColor(renderer, grid_cursor_ghost_color.r,
					grid_cursor_ghost_color.g,
					grid_cursor_ghost_color.b,
					grid_cursor_ghost_color.a);
				SDL_RenderFillRect(renderer, &grid_cursor_ghost);
			}
		}

		void drawNodes() {
			SDL_SetRenderDrawColor(renderer , starting_node_color.r ,
				starting_node_color.g , grid_cursor_color.b ,
				starting_node_color.a);

			// draw the starting_node first
			SDL_Rect snode = {
				.x = std::get<0>(starting_node) * grid_cell_size,
				.y = std::get<1>(starting_node) * grid_cell_size,
				.w = grid_cell_size,
				.h = grid_cell_size,
			};
			SDL_RenderFillRect(renderer , &snode);

			// then draw the target_node
			SDL_Rect tnode = {
				.x = std::get<0>(target_node) * grid_cell_size,
				.y = std::get<1>(target_node) * grid_cell_size,
				.w = grid_cell_size,
				.h = grid_cell_size,
			};

			SDL_SetRenderDrawColor(renderer, target_node_color.r,
				target_node_color.g, grid_cursor_color.b,
				target_node_color.a);
			SDL_RenderFillRect(renderer , &tnode);
		}

		void handleKeyboardEvents(SDL_Event event) {
			switch (event.key.keysym.sym) {
			case SDLK_w:
			case SDLK_UP:
				grid_cursor.y -= grid_cell_size;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				grid_cursor.y += grid_cell_size;
				break;
			case SDLK_a:
			case SDLK_LEFT:
				grid_cursor.x -= grid_cell_size;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				grid_cursor.x += grid_cell_size;
				break;
			case SDLK_f:
				disable_cells_mode = !disable_cells_mode;
				break;
			case SDLK_g:
				run_dijkstra();
				break;
			case SDLK_ESCAPE:
				quit = SDL_TRUE;
				break;
			}
		}

		void handleSDLEvents(const SDL_Event& event) {
			switch (event.type) {
			case SDL_KEYDOWN:
				handleKeyboardEvents(event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				grid_cursor.x = (event.motion.x / grid_cell_size) * grid_cell_size;
				grid_cursor.y = (event.motion.y / grid_cell_size) * grid_cell_size;

				if (event.button.button == SDL_BUTTON_LEFT) {
					if (disable_cells_mode)
						disable_selected_cell();
					else
						starting_node = std::make_pair(grid_cursor.x / grid_cell_size, grid_cursor.y / grid_cell_size);
				}

				else if (event.button.button == SDL_BUTTON_RIGHT)
					target_node = std::make_pair(grid_cursor.x / grid_cell_size , grid_cursor.y / grid_cell_size);
				break;
			case SDL_MOUSEMOTION: {
				grid_cursor_ghost.x = (event.motion.x / grid_cell_size) * grid_cell_size;
				grid_cursor_ghost.y = (event.motion.y / grid_cell_size) * grid_cell_size;
				if (!mouse_active)
					mouse_active = SDL_TRUE;
				break;
			}
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
					mouse_hover = SDL_TRUE;
				else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover)
					mouse_hover = SDL_FALSE;
				break;
			case SDL_QUIT:
				quit = SDL_TRUE;
				break;
			}

		}

		void run_dijkstra() {
			if (graph.get() == nullptr)
				exit(-1);

			int start = dijkstra::WeightedGraph::nodeIndex(std::get<0>(starting_node), std::get<1>(starting_node), grid_width);
			int end = dijkstra::WeightedGraph::nodeIndex(std::get<0>(target_node), std::get<1>(target_node), grid_width);
			
			dijkstra_solution = std::make_unique<std::vector<int>>(std::move(graph->shortestPath(start, end)));

			visualize_dijkstra = true;
		}

		/*
		* Disables the clicked cell in the graph
		*/
		void disable_selected_cell() {

			/*
			* Any given node can only have 4 neighbours:
			* * node - 1
			* * node + 1
			* * node + grid_width
			* * node - grid_width
			*/

			int node = dijkstra::WeightedGraph::nodeIndex(grid_cursor.x / grid_cell_size, grid_cursor.y / grid_cell_size, grid_width);

			int n1 = node - 1;
			int n2 = node + 1;
			int n3 = node + grid_width;
			int n4 = node - grid_width;

			if (n1 > 0 && n1 < index_to_coords_map->size()) {
				graph->disconnectNodes(node, n1);
			}
			if (n2 > 0 && n2 < index_to_coords_map->size()) {
				graph->disconnectNodes(node, n2);
			}
			if (n3 > 0 && n3 < index_to_coords_map->size()) {
				graph->disconnectNodes(node, n3);
			}
			if (n4 > 0 && n4 < index_to_coords_map->size()) {
				graph->disconnectNodes(node, n4);
			}

			std::cout << "Deleted node: " << node << " with neighbours: " << n1 << " " << n2 << " " << n3 << " " << n4 << '\n';
		}

	private:
		SDL_Rect grid_cursor;
		SDL_Rect grid_cursor_ghost;
		SDL_Color starting_node_color;
		SDL_Color target_node_color;
		SDL_Color grid_background;
		SDL_Color grid_line_color;
		SDL_Color grid_cursor_ghost_color;
		SDL_Color grid_cursor_color;

		int grid_width;
		int grid_height;
		int grid_cell_size;

		bool quit = false;
		bool mouse_active = false;
		bool mouse_hover = false;
		bool visualize_dijkstra = false;
		bool disable_cells_mode = false;

		std::tuple<int, int> starting_node{};
		std::tuple<int, int> target_node{};

		std::unique_ptr<dijkstra::WeightedGraph> graph;
		std::unique_ptr<std::vector<int>> dijkstra_solution;
		std::unique_ptr<std::vector<std::tuple<int, int>>> index_to_coords_map;

		int window_width;
		int window_height;

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
	};
}