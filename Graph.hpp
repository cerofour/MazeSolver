#pragma once

#include <vector>
#include <queue>
#include <climits>
#include <cassert>

namespace dijkstra {

	/**
	 * A weighted graph, implemented using a adjacency list.
	 */
	class WeightedGraph {
	public:
		WeightedGraph(const std::vector<std::vector<int>>&& adjList)
			:adjacencyList(std::move(adjList)) {
		}

	public:
		using NodeDistancePair = std::tuple<int, int>;

		struct Compare {
			bool operator()(const NodeDistancePair& a, const NodeDistancePair& b) {
				return std::get<1>(a) > std::get<1>(b);
			}
		};

	public:

		/*
		* Helper static methods.
		*/

		/*
		* Checks if x belongs to [low, high] (inclusive)
		*/
		static bool inRange(int x, int low, int high) {
			if (x >= low && x <= high)
				return true;
			return false;
		}

		/*
		* Each node located at(x, y) is indexed into the adjacencyList with this formula.
		*/
		static int nodeIndex(int x, int y, int w) {
			return x + y * w;
		}

		static std::vector<std::vector<int>> createAdjacencyList(int grid_width, int grid_height) {
			int graph_size = grid_width * grid_height;

			std::vector<std::vector<int>> adjacencyList(graph_size);

			for (int x = 0; x < grid_width; x++) {
				for (int y = 0; y < grid_height; y++) {
					int this_node = nodeIndex(x, y, grid_width);

					// calculate and store valid neighbours of this node
					const auto checkAndPushNeighbour =
						[&adjacencyList, &this_node, &grid_width]
						(int pos, int x, int y, int lo, int hi) {
						if (inRange(pos, lo, hi))
							adjacencyList[this_node].push_back(nodeIndex(x, y, grid_width));
						};

					checkAndPushNeighbour(x - 1, x - 1, y, 0, grid_width - 1);
					checkAndPushNeighbour(x + 1, x + 1, y, 0, grid_width - 1);
					checkAndPushNeighbour(y - 1, x, y - 1, 0, grid_height - 1);
					checkAndPushNeighbour(y + 1, x, y + 1, 0, grid_height - 1);
				}
			}

			return adjacencyList;
		}

	public:
		void disconnectNodes(int node, int neighbour) {

			auto search_and_delete = [](std::vector<int>& list, int target) {
					for (int i = 0; i < list.size(); i++) {
						if (list[i] == target) {
							list[i] = list.back();
							list.pop_back();
						}
					}
				};

			search_and_delete(adjacencyList[node], neighbour);
			search_and_delete(adjacencyList[neighbour], node);
		}

		void connectNodes(int node, int neighbour) {
			// nodes are already connected


			if (std::find(adjacencyList[node].begin(), adjacencyList[node].end(), neighbour) != adjacencyList[node].end())
				return;
			if(std::find(adjacencyList[neighbour].begin(), adjacencyList[neighbour].end(), node) != adjacencyList[neighbour].end())
				return;

			adjacencyList[neighbour].push_back(node);
			adjacencyList[node].push_back(neighbour);
		}

		std::vector<int> shortestPath(int start, int end) {
			std::vector<int> parents = dijkstra(start);
			std::vector<int> path{};
			for (int i = end; i != -1; i = parents[i]) {
				path.push_back(i);
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		std::vector<int> dijkstra(int start) {
			std::priority_queue<NodeDistancePair, std::vector<NodeDistancePair>, Compare> queued_nodes{};
			std::vector<int> distances(adjacencyList.size(), INT_MAX);
			std::vector<int> parents(adjacencyList.size(), -1);

			queued_nodes.push(std::make_pair(start, 0));
			distances[start] = 0;

			while (!queued_nodes.empty()) {
				auto [node, current_distance] = queued_nodes.top();
				queued_nodes.pop();

				for (int i = 0; i < adjacencyList[node].size(); i++) {
					int next_node = adjacencyList[node][i];
					int weight = 1; /* fixed size because we are working in a grid */

					if (distances[next_node] > (distances[node] + weight)) {
						distances[next_node] = (distances[node] + weight);
						parents[next_node] = node;
						queued_nodes.push(std::make_pair(next_node, distances[next_node]));
					}
				}
			}
			return parents;
		}

	private:
		std::vector<std::vector<int>> adjacencyList;
	};
};