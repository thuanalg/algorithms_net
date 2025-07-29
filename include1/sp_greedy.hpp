
#ifndef __sp_greedy__
#define __sp_greedy__
#include "sp_numberic.hpp" 
#include <type_traits> //C++11
#include <iostream> //C++98
#include <concepts> //C++20

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
	std::vector<std::vector<int>> direction = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
	std::vector<std::vector<bool>> mtrace( m, std::vector<bool>(n, false));
	for (i = 0; i < m; ++i) {
		
		for (j = 0; i < n; ++j) {
			if (!mtrace[i][j])
				continue;
			if (!vec[i][j])
				continue;
			std::vector <sp_numberic<int>> island;
			if (vec[i][j]) {
				sp_numberic<int> pos;
				pos.x1 = i;
				pos.x2 = j;
				mtrace[i][j] = true;
				island.emplace_back(pos);
				ouput.emplace_back(std::move(island));
			}
			mtrace[i][j] = true;
		}
	}
	return ret;
}
#endif // __sp_greedy__
