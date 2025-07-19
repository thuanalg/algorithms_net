#include <sp_cppalg.h>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int sp_cppalg_hello() {
	return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
// 0. Largest problem
// 1. List all islands out
// 2. List the beggest island out
int
sp_cppalg_greedy(int a, int b,
	SP_CppCALLBACK_FUNCTION f, void* obj) 
{
	int ret = 0;
	return ret;
}
// 0. Main problem: Identify islands in a grid
// 1. List all islands
// 2. Find the largest island (by area = number of land cells)

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

using Grid = vector<vector<int>>;
using Coord = pair<int, int>;

const vector<Coord> directions = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1} // Up, Down, Left, Right
};

// Depth-First Search to collect all connected land cells
void
dfs(int x, int y, const Grid &grid, vector<vector<bool>> &visited,
    vector<Coord> &island)
{
	int rows = grid.size();
	int cols = grid[0].size();

	if (x < 0 || y < 0 || x >= rows || y >= cols)
		return;
	if (grid[x][y] == 0 || visited[x][y])
		return;

	visited[x][y] = true;
	island.emplace_back(x, y);

	for (auto [dx, dy] : directions) {
		dfs(x + dx, y + dy, grid, visited, island);
	}
}

// Find all islands in the grid
vector<vector<Coord>>
listAllIslands(const Grid &grid)
{
	int rows = grid.size();
	int cols = grid[0].size();
	vector<vector<bool>> visited(rows, vector<bool>(cols, false));
	vector<vector<Coord>> islands;

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			if (grid[i][j] == 1 && !visited[i][j]) {
				vector<Coord> island;
				dfs(i, j, grid, visited, island);
				islands.push_back(move(island));
				//islands.emplace_back(move(island));
			}

	return islands;
}

int
main()
{
	Grid grid = {
	    {1, 1, 0, 0, 0},
	    {1, 0, 0, 1, 1},
	    {0, 0, 1, 1, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	};

	// 1. List all islands
	auto islands = listAllIslands(grid);
	cout << "Found " << islands.size() << " islands:\n";
	for (int i = 0; i < islands.size(); ++i) {
		cout << "- Island " << i + 1 << " with " << islands[i].size()
		     << " cells: ";
		for (auto [x, y] : islands[i])
			cout << "(" << x << "," << y << ") ";
		cout << '\n';
	}

	// 2. Find the largest island
	auto maxIslandIt = max_element(islands.begin(), islands.end(),
	    [](const auto &a, const auto &b) { return a.size() < b.size(); });

	if (maxIslandIt != islands.end()) {
		cout << "The largest island has " << maxIslandIt->size()
		     << " cells: ";
		for (auto [x, y] : *maxIslandIt)
			cout << "(" << x << "," << y << ") ";
		cout << '\n';
	}

	return 0;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/