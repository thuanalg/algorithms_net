
#ifndef __sp_greedy__
#define __sp_greedy__
#include "sp_numberic.hpp" 
#include <type_traits> //C++11
#include <iostream> //C++98
#include <concepts> //C++20

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
		if (!mtrace[u][v])
			continue;
		sp_numberic<int> pos(u, v);
		island.emplace_back(pos);
		mtrace[u][v] = true;
		sp_greedy_recursive(vec, island, i, j, mtrace);
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
		
		for (j = 0; i < n; ++j) {
			if (!mtrace[i][j])
				continue;
			if (!vec[i][j])
				continue;
			std::vector <sp_numberic<int>> island;
			if (vec[i][j]) {
				sp_numberic<int> pos(i, j);
				mtrace[i][j] = true;
				island.emplace_back(pos);
				sp_greedy_recursive(vec, island, i, j, mtrace);
				ouput.emplace_back(std::move(island));
			}
			mtrace[i][j] = true;
		}
	}
	return ret;
}
#endif // __sp_greedy__
