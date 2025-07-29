#ifndef __sp_dijkstra__
#define __sp_dijkstra__

//#include "sp_numberic.hpp"
#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

using Grid = vector<vector<int>>;
const int INF = numeric_limits<int>::max();

std::vector<int>
dijkstra(const Grid &grid, int s, vector<int> &parent, std::vector<int> &output)
{
	size_t n = grid.size();
	vector<int> dist(n, INF);
	vector<bool> visited(n, false);
	std::priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

	dist[s] = 0;
	parent[s] = -1; // root has no parent
	pq.emplace(0, s);

	while (!pq.empty()) {
		auto [d, u] = pq.top();
		pq.pop();
		if (visited[u])
			continue;
		visited[u] = true;

		for (int v = 0; v < n; ++v) {
			if (grid[u][v] == INF) {
				continue;
			}
			if (dist[v] > dist[u] + grid[u][v]) {
				dist[v] = dist[u] + grid[u][v];
				parent[v] = u; // save parent
				pq.emplace(dist[v], v);
			}
		}
	}
	std::
	return dist;
}

#endif // __sp_dijkstra__
