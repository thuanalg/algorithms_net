#ifndef __sp_greedy__
#define __sp_greedy__

#include "sp_numberic.hpp"
#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

template <typename T>
int
sp_greedy_recursive(std::vector<std::vector<T>> &vec,
    std::vector<sp_numberic<int>> &island, int i, int j,
    std::vector<std::vector<bool>> &mtrace)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	std::vector<std::vector<int>> direction = {
	    {0, 1}, {0, -1}, {1, 0}, {-1, 0}};

	for (auto d : direction) {
		int u = d[0] + i;
		int v = d[1] + j;

		if (u < 0 || v < 0)
			continue;
		if (u >= vec.size() || v >= vec[0].size())
			continue;
		if (!vec[u][v])
			continue;
		if (mtrace[u][v])
			continue;

		sp_numberic<int> pos(u, v);
		island.emplace_back(pos);
		mtrace[u][v] = true;
		sp_greedy_recursive(vec, island, u, v, mtrace);
	}
	return ret;
}

template <typename T>
int
sp_greedy(std::vector<std::vector<T>> &vec,
    std::vector<std::vector<sp_numberic<int>>> &ouput)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	int m = vec.size();
	if (m < 1)
		return ret;
	int n = vec[0].size();

	std::vector<std::vector<bool>> mtrace(m, std::vector<bool>(n, false));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if (mtrace[i][j])
				continue;
			if (!vec[i][j]) {
				mtrace[i][j] = true;
				continue;
			}

			std::vector<sp_numberic<int>> island;
			island.emplace_back(i, j);
			mtrace[i][j] = true;

			sp_greedy_recursive(vec, island, i, j, mtrace);
			ouput.emplace_back(std::move(island));
		}
	}

	// C++20 sort 
	std::ranges::sort(ouput,
	    [](const auto &a, const auto &b) { return a.size() < b.size(); });

	return ret;
}

inline int
sp_greedy_test(std::vector<std::vector<int>> &vec)
{
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	for (auto &row : vec)
		for (auto &val : row)
			val = std::rand() % 3 ? 0 : 1;

	for (const auto &row : vec) {
		for (int v : row)
			std::cout << v << "\t";
		std::cout << "\n";
	}

	std::vector<std::vector<sp_numberic<int>>> ouput;
	sp_greedy(vec, ouput);

	std::cout << "Output = " << ouput.size() << "\n";
	return 0;
}

#endif // __sp_greedy__
