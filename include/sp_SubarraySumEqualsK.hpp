#ifndef __sp_SubarraySumEqualsK__
#define __sp_SubarraySumEqualsK__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>

#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

template <typename T> using EqualKcoord = std::pair<int, int>;
template <typename T>

inline int
sp_SubarraySumEqualsK(
    std::vector<T> &vec, T &k, std::vector<EqualKcoord<int>> &output)
	requires std::is_arithmetic_v<T>
{
	std::unordered_map<T, int> umap;
	T sum = 0;
	int i = 0;
	for (auto &v : vec) {
		sum += v;
		if (sum == k) {
			output.emplace_back(0, i);
		}
		if (umap.find(sum - k) != umap.end()) 
		{
			int j = i;
			T sumj = 0;
			for (; j >= 0; --j) 
			{
				sumj += vec[j];
				if (sumj == k) 
				{
					output.emplace_back(j, i);
					break;
				}
			}
		}
		umap[sum]++;
		++i;
	}
	return 0;
}
inline int
sp_SubarraySumEqualsK_test()
{
	return 0;
}
#endif // __sp_SubarraySumEqualsK__
