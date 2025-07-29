
#ifndef __sp_greedy__
#define __sp_greedy__
#include "sp_numberic.hpp" 
#include <type_traits> //C++11
#include <iostream> //C++98
#include <concepts> //C++20
#include <ranges> //C++20
#include <algorithm>
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
		int u, v;
		u = d[0] + i;
		v = d[1] + j;
		if (u < 0 || v < 0)
			continue;
		if (u >= vec.size() || v >= vec[0].size())
			continue;
		if (!vec[u][v])
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
template<typename T>
int
sp_greedy(std::vector<std::vector<T>> &vec,
    std::vector<std::vector<sp_numberic<int>>> &ouput)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	int m = vec.size();
	int i, j;
	if (m < 1) {
		return ret;
	}
	int n = vec[0].size();
	
	std::vector<std::vector<bool>> mtrace( m, std::vector<bool>(n, false));
	for (i = 0; i < m; ++i) {
		
		for (j = 0; j < n; ++j) {
			if (mtrace[i][j])
				continue;
			if (!vec[i][j]) {
				mtrace[i][j] = true;
				continue;
			}
			std::vector <sp_numberic<int>> island;
			sp_numberic<int> pos(i, j);
			mtrace[i][j] = true;
			island.emplace_back(pos);
			sp_greedy_recursive(vec, island, i, j, mtrace);
			// C++20
			//td::ranges::sort(
			//   island, [](const auto &a, const auto &b) {
			//	    if (a.x1() < b.x1) {
			//		    return a.x1() < b.x1;
			//	    }
			//	    else if (a.x1() > b.x1) {
			//		    return a.x1() > b.x1;
			//	    }
			//	    return !!0;
			//   });
			ouput.emplace_back(std::move(island));
			
		}
	}
	// C++20
	std::ranges::sort(ouput, [](const auto &a, const auto &b) { 
		return a.size() < b.size(); });
	return ret;
}
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
int
sp_greedy_test(std::vector<std::vector<int>> &vec)
{
	int ret = 0;
	for (int i = 0; i < vec.size(); ++i) {
		for (int j = 0; j < vec[0].size(); ++j) {
			int k = std::rand() % 3;
			vec[i][j] = k ? 0 : 1;
		}
	}
	for (int i = 0; i < vec.size(); ++i) {
		std::cout << "\n";
		for (int j = 0; j < vec[0].size(); ++j) {
			std::cout << vec[i][j] << "\t";
		}
	}
	std::cout << "\n";
	std::vector<std::vector<sp_numberic<int>>> ouput;
	sp_greedy(vec, ouput);
	return ret;
}
#endif // __sp_greedy__
