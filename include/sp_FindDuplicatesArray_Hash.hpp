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
	//for (auto v : vec) {
	//	output[v]++;
	//}
	return 0;
}

#endif // __sp_FindDuplicatesArray_Hash__
