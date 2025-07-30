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
sp_print_path(int v, const vector<int> &parent,
    std::vector<int> &apath)
{
	apath.insert(apath.begin(), v);
	if (parent[v] == -1) {
		//cout << v;
		return;
	}
	sp_print_path(parent[v], parent, apath);
	//cout << " -> " << v;
}
inline void
exportToGraphviz(const Grid &grid, std::string &output)
{
	int n = grid.size();
	int INF = SP_INF_DIJKSTRA;
	output += "digraph G {\n";
	output += "    rankdir = LR;\n";
	output += "    node [shape=circle];\n\n";



	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j && grid[i][j] != INF) {
				char data[8];
				snprintf(data, 8, "%d", i);

				output += "    ";
				output += data;
				output += " -> ";
				snprintf(data, 8, "%d", j);
				output += data;
				output += " [label = \"";
				snprintf(data, 8, "%d", grid[i][j]);
				output += data;
				output += "\"];\n";

			}
		}
	}
	output += "}\n";
}
inline int
sp_dijkstra_test()
{
	int ret = 0;
	int INF = SP_INF_DIJKSTRA;
	// http://www.webgraphviz.com/
#if 1
	Grid grid = {
	    {0, 1, 4, INF, INF, 10}, // 0
	    {INF, 0, 1, INF, 1, INF}, // 1
	    {INF, INF, 0, 1, 1, 12}, // 2
	    {INF, INF, INF, 0, INF, 2}, // 3
	    {INF, INF, INF, 10, 0, 6}, // 4
	    {INF, INF, INF, INF, INF, 0}, // 5
	};
#else
	Grid grid = {
	    // 0   1    2    3    4    5    6    7    8    9   10   11   12   13
	    // 14   15   16   17   18   19
	    {0, 4, INF, 7, INF, INF, INF, INF, 9, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, INF}, // 0
	    {INF, 0, 3, INF, INF, 5, INF, INF, INF, INF, INF, 7, INF, INF,
		INF, INF, INF, INF, INF, INF}, // 1
	    {INF, INF, 0, INF, INF, INF, 2, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, INF, INF}, // 2
	    {INF, INF, INF, 0, 6, INF, INF, 1, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, INF, INF}, // 3
	    {INF, INF, INF, INF, 0, INF, INF, INF, INF, 3, INF, INF, INF, INF,
		INF, INF, INF, INF, INF, INF}, // 4
	    {INF, INF, INF, INF, INF, 0, INF, INF, INF, INF, 8, INF, INF, INF,
		INF, INF, INF, INF, INF, INF}, // 5
	    {INF, INF, INF, INF, INF, INF, 0, INF, INF, INF, INF, INF, 4, INF,
		INF, INF, INF, INF, INF, INF}, // 6
	    {INF, INF, INF, INF, INF, INF, INF, 0, INF, INF, INF, INF, INF, 2,
		INF, INF, INF, INF, INF, INF}, // 7
	    {INF, INF, INF, INF, INF, INF, INF, INF, 0, INF, INF, INF, INF, INF,
		6, INF, INF, INF, INF, INF}, // 8
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF, INF, INF, INF,
		INF, 7, INF, INF, INF, INF}, // 9
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF, INF, INF,
		INF, INF, 1, INF, INF, INF}, // 10
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF, INF,
		INF, INF, INF, 5, INF, INF}, // 11
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF,
		INF, INF, INF, INF, 2, INF}, // 12
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 0,
		INF, INF, INF, INF, INF, 9}, // 13
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, 0, INF, INF, INF, INF, INF}, // 14
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, 0, INF, INF, INF, INF}, // 15
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, 0, INF, INF, INF}, // 16
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, 0, INF, INF}, // 17
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, INF, 0, 1}, // 18
	    {INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,
		INF, INF, INF, INF, INF, INF, 0} // 19
	};
	//https://drive.google.com/file/d/1t5f5We74UznKFsSoLotXjWxLoJgPingv/view?usp=sharing
#endif
	// Generate data for Graphviz like:
	std::string str;
	exportToGraphviz(grid, str);
	std::vector<std::vector<int>> pathsoutput;

	vector<int> parent(grid.size(), -1);
	std::vector<int> output(grid.size(), SP_INF_DIJKSTRA);
	ret = sp_dijkstra(grid, 0, parent, output);

	int i = 0;
	for (auto v : output) {
		if (v >= SP_INF_DIJKSTRA) {
			++i;
			continue;
		}
		
		std::vector<int> apath;
		sp_print_path(i, parent, apath);
		std::cout << "\nPath: ";
	
		for (auto edge : apath) {
			std::cout << edge << " -->> ";
		}
		std::cout << " End ";
		pathsoutput.emplace_back(std::move(apath));
		
		std::cout << "\t\t### distance: " << v << "\n";
		++i;
	}

	return ret;
}

#endif // __sp_dijkstra__
