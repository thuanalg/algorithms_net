#ifndef __sp_FindDuplicatesArray_Hash__
#define __sp_FindDuplicatesArray_Hash__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <map>
#include <iostream>
#include <vector>

using namespace std;


template <typename T>
inline int
sp_FindDuplicatesArray_Hash(std::vector<T> &vec, std::map<T, int> &output)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	if (!vec.size()) {
		return ret;
	}
	for (auto v : vec) {
		output[v]++;
	}
	return 0;
}

inline int
sp_FindDuplicatesArray_Hash_test()
{
	std::vector<int> vec = { 10, 9, 2, 5, 2, 7, 101, 101, 101, 18, 101,
		101, 101, 101, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 3, 7,
		7, 7, 8, 9, 7, };
	std::map<int, int> freq;
	sp_FindDuplicatesArray_Hash(vec, freq);
	std::pair<int, int> max_freq;
	std::cout << "\n";
	for (auto &[k, n] : freq) {
		std::cout << "(" << k << "," << n << ")\t";
		if (max_freq.second < n) {
			max_freq.second = n;
			max_freq.first = k;
		}
	}
	std::cout << "\n";
	std::cout << "(" << max_freq.first << "," << max_freq.second << ")\t";
	std::cout << "\n";
	return 0;
}

#endif // __sp_FindDuplicatesArray_Hash__
