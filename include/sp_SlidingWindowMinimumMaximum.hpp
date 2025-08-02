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
#include <deque>
#include <unordered_map>
using namespace std;

template <typename T>
inline int
sp_SlidingWindowMaximum( std::vector<T> &arr, int k)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	std::deque<T> dq;
	vector<T> res;

	for (int i = 0; i < arr.size(); ++i) {
		if (!dq.empty() && dq.front() <= i - k)
			dq.pop_front();

		while (!dq.empty() && arr[dq.back()] <= arr[i])
			dq.pop_back();

		dq.push_back(i);

		if (i >= k - 1)
			res.push_back(arr[dq.front()]);
	}

	//return res;
	return 0;
}

template <typename T>
inline int
sp_SlidingWindowMinimum(std::vector<T> &arr, int k)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	deque<T> dq;
	vector<T> res;
	
	for (int i = 0; i < arr.size(); ++i) {
		if (!dq.empty() && dq.front() <= i - k)
			dq.pop_front();
	
		while (!dq.empty() && arr[dq.back()] >= arr[i])
			dq.pop_back();
	
		dq.push_back(i);
	
		if (i >= k - 1)
			res.push_back(arr[dq.front()]);
	}
	
	return 0;
}

inline int
sp_SlidingWindowMinimumMaximum_test()
{
	std::vector<int> vec = { 5, 7, 1, 4, 3, 6, 2, 9, 2 };
	sp_SlidingWindowMaximum(vec, 3);
	return 0;
}
#endif // __sp_SlidingWindowMinimumMaximum__
