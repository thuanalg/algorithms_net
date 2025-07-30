#ifndef __sp_LongestIncreasingSubsequence__
#define __sp_LongestIncreasingSubsequence__

//#include "sp_numberic.hpp"
#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

template <typename T>
inline int 
sp_clean_trash(std::vector<std::vector<T>> &vec)
	requires(std::is_arithmetic_v<T> || std::totally_ordered<T>)
{
	int ret = 0;

	if (vec.size() < 1) {
		return ret;
	}
	// C++20
	std::ranges::sort(vec, [](const auto &a, const auto &b) {
		if (a.size() != b.size())
			return a.size() < b.size();
		int i = 0, n = a.size();
		for (i = 0; i < n; ++i) {
			if (a[i] != b[i]) {
				return a[i] < b[i];
			}
		}
		return !!0;
	});
#if 0
	for (size_t index = 0; index < vec.size() - 1; ++index) {
		if (vec[index].size() == vec[index + 1].size()) {
			size_t i = vec[index].size() - 1;
			size_t j = vec[index + 1].size() - 1;
			size_t fd = index;
			if (vec[index][i] < vec[index + 1][j]) {
				fd = index + 1;
			}
			vec.erase(vec.begin() + fd);
		}
	}
#endif
	for (size_t index = 0; index < vec.size() - 1; ++index) {
		if (vec[index].size() == vec[index + 1].size()) {
			int tryremove = 1;
			size_t i = vec[index].size() - 1;
			size_t j = vec[index + 1].size() - 1;
			for (int k = 0; k < i; ++k) {
				if (vec[index][k] != vec[index + 1][k]) {
					tryremove = 0;
					break;
				}
			}
			if (!tryremove) {
				continue;
			}
			size_t fd = index;
			if (vec[index][i] < vec[index + 1][j]) {
				fd = index + 1;
			}
			vec.erase(vec.begin() + fd);
			index--;
		}
	}
#if 1
	for (int i = 0; i < vec.size() - 1; ++i) {
		if (vec[i].empty()) {
			continue;
		}
		auto iti = vec[i].end();
		iti--;
		T a = *iti;
		for (int j = i + 1; j < vec.size(); ++j) {
			if (vec[j].empty()) {
				break;
			}
			auto itj = vec[j].end();
			itj--;
			T b = *itj;
			if (a == b) {
				int fd = i;
				if (vec[i].size() >= vec[j].size()) {
					fd = j;
				}
				vec.erase(vec.begin() + fd);
			}
		}
	}
#endif
	return ret;
}

template <typename T>
int
sp_LongestIncreasingSubsequence(const std::vector<T> &nums, std::vector<T> &output)
	requires(std::is_arithmetic_v<T> || std::totally_ordered<T>)
{
	int ret = 0;
	std::vector<T> lis; //
	std::vector<std::vector<T>> arrlis; //
	std::vector<std::vector<T>> arrlistmp; //
	if (nums.size() == 0) {
		return ret;
	}
	// https://en.wikipedia.org/wiki/Longest_increasing_subsequence
	arrlis.push_back(lis);

	for (auto &num : nums) {
		// if (num == 11) {
		//	//int a = num;
		// }
		arrlistmp.clear();
		for (auto &tmp : arrlis) {
			auto it = std::lower_bound(tmp.begin(), tmp.end(), num);
			if (it == tmp.end()) {
				tmp.push_back(num); // Expand list
			} else {
				auto testit = tmp.end();
				testit--;
				if (*testit == num && 1) {
					tmp.push_back(num); // Expand list
				} else {
					std::vector<T> copied(tmp.begin(), it);
					copied.push_back(num);
					arrlistmp.push_back(std::move(copied));
				}
			}
		}
		if (arrlistmp.size()) {
			sp_clean_trash(arrlistmp);
			std::copy(arrlistmp.begin(), arrlistmp.end(),
			    std::back_inserter(arrlis));
		}
		sp_clean_trash(arrlis);
	}

	if (arrlis.size()) {
		//lis.clear();
		auto it = arrlis.end();
		--it;
		//lis = *it;
		output.clear();
		output = *it;
	}

	return (int)output.size();
}
int
sp_LongestIncreasingSubsequence_test()
{
#if 1
	std::vector<double> output;
	//std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101, 101, 101, 101, 101, 101,
	//	15, 16, 17, 19, 20, 1, -100, 2, 3, 4, 5, 6, 7, 7.1, 7.2, 8, 9.0,10,   };
	std::vector<double> data = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
	// https://en.wikipedia.org/wiki/Longest_increasing_subsequence
#else
	std::vector<sp_numberic<double>> output;
	std::vector<sp_numberic<double>> data = {{0.0, 0.2}, {8, 0}, {4, 0},
	    {12, 0}, {2, 0}, {10, 0}, {6, -1}, {6, -2.1}, {14, 0}, {1, -1.1},
	    {9, 0}, {5, 0}, {13, 0}, {3, 0}, {11, 0}, {7, 0}, {15, 0}};
#endif
	int length = sp_LongestIncreasingSubsequence(data, output);
	std::cout << "Length of LIS: " << length << '\n';
	std::cout << std::endl;
	int i = 0;
	for (auto v : output) {
		
		if (!i) {
			std::cout << v;
			++i;
			continue;
		} else {
			std::cout << " -> " << v;
		}
		++i;
	}
	std::cout << std::endl;
	return 0;
}
#endif // __sp_LongestIncreasingSubsequence__
