#ifndef __sp_MaximumProfitStockTrading__
#define __sp_MaximumProfitStockTrading__

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

template <typename T> using ProfitCoord = std::pair<int, int>;
template <typename T>

inline int
sp_MaximumProfitStockTrading(
    std::vector<T> &vec, T &output, ProfitCoord<int> &index)
	requires std::is_arithmetic_v<T>
{
	if (vec.size() < 2) {
		return 0;
	}
	T min_price = vec[0], max_profit = vec[0] - vec[0];
	int i = 0;
	int start = -1;
	int end = -1;
	ProfitCoord<int> prev(-1, -1), curr(-1, -1);
	for (const auto &v : vec) {
		if (i == 0) {
			start = 0;
			end = 0;
			prev.first = i;
			curr.first = i;			
			min_price = v;
			max_profit = (v - v);
			++i;
			continue;
		}
		if (v < min_price) {
			min_price = v;
			if (prev.first < 0) {
				prev.first = i;
				curr.first = i;
			} else if (curr.first == prev.first) {
				curr.first = i;
			} else if (curr.first > prev.first) {
				// if ()
				if (curr.first < curr.second) {
					prev.first = curr.first;
				}
				curr.first = i;
			}
		} else if (v - min_price > max_profit) {
			max_profit = (v - min_price);
			end = i;
			if (prev.second < 0) {
				prev.second = i;
				curr.second = i;
			} else if (prev.second == curr.second) {
				curr.second = i;
			} else if (prev.second < curr.second) {
				prev.second = curr.second;
				curr.second = i;
			}
		}
		++i;
	}
	output = max_profit;
	if (curr.second > curr.first) {
		index = curr;
	} 
	else if (curr.second > prev.first) {
		index.first = prev.first;
		index.second = curr.second;
	} else {
		index.first = -1;
		index.second = -1;
	}
	return 0;
}


inline int
sp_MaximumProfitStockTrading_test()
{
	ProfitCoord<int> index;
#if 0
	double k = 0;
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101,
		101, 101, 101, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 7,
		7.1, 7.2, 8, 9.0 };
	// std::vector<float> data = {10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101,
	// 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, };
#else
	double k = 0;
	std::vector<double> data = {
	    7, 0, 7, 7, 3, 7, 101, 18, 10, 101, 101, -1, 101, -2};
#endif

	int ret = sp_MaximumProfitStockTrading(data, k, index);
	return ret;
}
#endif // __sp_MaximumProfitStockTrading__
