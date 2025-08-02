#ifndef __sp_SlidingWindowMinimumMaximum__
#define __sp_SlidingWindowMinimumMaximum__

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

template <typename T>
inline int
sp_SlidingWindowMinimumMaximum( std::vector<T> &vec)
	requires std::is_arithmetic_v<T>
{
	return 0;
}


inline int
sp_SlidingWindowMinimumMaximum_test()
{
	return 0;
}
#endif // __sp_SlidingWindowMinimumMaximum__
