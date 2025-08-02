#ifndef __sp_FindDuplicatesArray__
#define __sp_FindDuplicatesArray__

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
#include <unordered_map>
using namespace std;

template <typename T> using HashcCoord = std::pair<T, T>;

template <typename T>
inline int
sp_FindDuplicatesArray(
    std::vector<T> &vec, T &k, std::vector<HashcCoord<T>> &output)
	requires std::is_arithmetic_v<T>
{
	return 0;
}

inline int
sp_FindDuplicatesArray_test()
{
	return 0;
}

#endif // __sp_FindDuplicatesArray__
