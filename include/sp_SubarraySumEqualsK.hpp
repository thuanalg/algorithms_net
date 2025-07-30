#ifndef __sp_SubarraySumEqualsKadane__
#define __sp_SubarraySumEqualsKadane__

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
sp_SubarraySumEqualsKadane_test(
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
sp_SubarraySumEqualsKadane_test()
{
#if 0
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101,
		101, 101, 101, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 7,
		7.1, 7.2, 8, 9.0 };
#else
	std::vector<int> data = {7, 0, -7, 7, 3, 7, 101, 18, 101, 101, 15};
#endif
	std::vector<EqualKcoord<int>> output;
	int k = 10;
	int ret = sp_SubarraySumEqualsKadane_test(data, k, output);
	return 0;
}
#endif // __sp_SubarraySumEqualsKadane__
