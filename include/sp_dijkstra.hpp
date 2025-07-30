#ifndef __sp_dijkstra__
#define __sp_dijkstra__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>

using namespace std;

using Grid = vector<vector<int>>;
const int SP_INF_DIJKSTRA = numeric_limits<int>::max();

inline int
sp_dijkstra(const Grid &grid, int s, vector<int> &parent, std::vector<int> &output)
{
	int ret = 0;
	size_t n = grid.size();
	std::vector<int> dist(n, SP_INF_DIJKSTRA);
	std::vector<bool> visited(n, false);
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
			if (grid[u][v] == SP_INF_DIJKSTRA) {
				continue;
			}
			if (dist[v] > dist[u] + grid[u][v]) {
				dist[v] = dist[u] + grid[u][v];
				parent[v] = u; // save parent
				pq.emplace(dist[v], v);
			}
		}
	}
	output = dist;
	return ret;
}
//http://www.webgraphviz.com/
inline void
sp_print_path(int v, const vector<int> &parent)
{
	if (parent[v] == -1) {
		cout << v;
		return;
	}
	sp_print_path(parent[v], parent);
	cout << " -> " << v;
}
inline int
sp_dijkstra_test()
{
	int ret = 0;
	int INF = SP_INF_DIJKSTRA;
	// http://www.webgraphviz.com/
	Grid grid = {
	    {0, 1, 4, INF, INF, 1}, // 0
	    {INF, 0, 1, INF, 1, INF}, // 1
	    {INF, INF, 0, 1, 1, 12}, // 2
	    {INF, INF, INF, 0, INF, 2}, // 3
	    {INF, INF, INF, 10, 0, 6}, // 4
	    {INF, INF, INF, INF, INF, 0}, // 5
	};
	vector<int> parent(grid.size(), -1);
	std::vector<int> output(grid.size(), SP_INF_DIJKSTRA);
	ret = sp_dijkstra(grid, 0, parent, output);
	//sp_print_path(0, parent);
	//for (int i = 0; i < dist.size(); ++i) {
	//	// if (i < dist.size() - 1) {
	//	//	continue;
	//	// }
	//	cout << "Distance to " << i << ": ";
	//	if (dist[i] == INF) {
	//		cout << "-1 (unreachable)\n";
	//	} else {
	//		cout << dist[i] << ", Path: ";
	//		sp_print_path(v, parent);
	//		cout << "\n";
	//	}
	//} 
	int i = 0;
	for (auto v : output) {
		if (v >= SP_INF_DIJKSTRA) {
			++i;
			continue;
		}
		std::cout << v << ", Path: ";
		sp_print_path(i, parent);
		++i;
	}
	return ret;
}

#endif // __sp_dijkstra__
