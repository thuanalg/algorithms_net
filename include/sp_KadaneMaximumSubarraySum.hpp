#ifndef __sp_KadaneMaximumSubarraySum__
#define __sp_KadaneMaximumSubarraySum__

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

template <typename T> using KadaneIndexPair = std::pair<int, int>;

template <typename K, typename V>
using KadaneData = std::map<K, V, std::greater<>>;

template <typename T>

inline int
sp_KadaneMaximumSubarraySum(
    std::vector<T> &data, KadaneData<T, KadaneIndexPair<int>> &output)
	requires std::is_arithmetic_v<T>
{
	// https://en.wikipedia.org/wiki/Maximum_subarray_problem
	// https://en.wikipedia.org/wiki/Maximum_subarray_problem#Kadane's_algorithm
	// C++20
	int ret = 0;
	int s = 0;
	int i = 0;
	output.clear();
	// int arr[] = {-2, -3, 4, -1, -2, 10, 5, -3};
	do {
		if (data.size() < 1) {
			ret = -1;
			break;
		}
//		auto MyaddLamda = []<typename U>(U &a, U &b)
//			requires(std::is_same<std::remove_cvref_t<T>, U>::value)
//			
//		{ a += b; };
		auto it = data.begin();
		T max_so_far = *it;
		T max_ending_here = *it;
		for (auto v : data) {
			if (!i) {
				++i;
				continue;
			}
			if (max_ending_here < 0) {
				max_ending_here = v;
				s = i;
			} else {
				max_ending_here += v;
				// MyaddLamda(max_ending_here, v);
			}
			if (max_ending_here > max_so_far) {
				max_so_far = max_ending_here;
				// Memorize a pair
				output[max_so_far] = std::make_pair(s, i);
			}
			++i;
		}
	} while (0);
	// int arr[] = {-2, -3, 4, -1, -2, 1, 5, -3};
	return ret;
}
//template <typename T>
inline int
sp_KadaneMaximumSubarraySum_test()
{
#if 1
	std::vector<double> dta = { -2, -3, 4, -1, -2, 10, 5.121, -3, 4, 0, -1, 2.0, -20, };
#else 
	std::vector<double> dta = {100, -3, 4, -1, -2, 1, 5.121, -3};
#endif
	KadaneData<double, KadaneIndexPair<int>> output;
	int ret = sp_KadaneMaximumSubarraySum(dta, output);
	for (auto it : output) {
		std::cout << "val: " << it.first << ", \tindex: ("
			  << it.second.first << ", " << it.second.second << ")."
			  << std::endl;
	}

	return 0;
}

#endif // __sp_KadaneMaximumSubarraySum__
