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
sp_SlidingWindowMaximum(std::vector<T> &vec, int k, vector<T> &output)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	std::deque<T> dq;

	for (int i = 0; i < vec.size(); ++i) {
		if (!dq.empty() && dq.front() <= i - k)
			dq.pop_front();

		while (!dq.empty() && vec[dq.back()] <= vec[i])
			dq.pop_back();

		dq.push_back(i);

		if (i >= k - 1)
			output.push_back(vec[dq.front()]);
	}

	return 0;
}

template <typename T>
inline int
sp_SlidingWindowMinimum(std::vector<T> &vec, int k, vector<T> &output)
	requires std::is_arithmetic_v<T>
{
	int ret = 0;
	deque<T> dq;
	
	for (int i = 0; i < vec.size(); ++i) {
		if (!dq.empty() && dq.front() <= i - k)
			dq.pop_front();
	
		while (!dq.empty() && vec[dq.back()] >= vec[i])
			dq.pop_back();
	
		dq.push_back(i);
	
		if (i >= k - 1)
			output.push_back(vec[dq.front()]);
	}
	
	return 0;
}

inline int
sp_SlidingWindowMinimumMaximum_test()
{
	std::vector<int> vec = { 5, 7, 1, 4, 3, 6, 2, 9, 2 };
	std::vector<int> output;
	sp_SlidingWindowMaximum(vec, 3, output);
	output.clear();
	sp_SlidingWindowMinimum(vec, 3, output);
	return 0;
}
#endif // __sp_SlidingWindowMinimumMaximum__
