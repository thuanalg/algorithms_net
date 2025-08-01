#ifndef __sp_PartitionProblem__
#define __sp_PartitionProblem__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <numeric>   // C++98 (ISO/IEC 14882:1998)

//#pragma warning(disable : 4146)
//#include <gmp.h>

using namespace std;

// arr[i] > 0 
// arr[i], integer
//template <typename T>
inline int
sp_PartitionProblem(std::vector<int> &arr, 
	bool &result, int kVal, std::vector<int> &output_track)
// requires std::is_arithmetic_v<T>  || 
// std::is_same<std::remove_extent_t<mpz_t>, T>::value
{
	//https://www.geeksforgeeks.org/dsa/partition-problem-dp-18/
	//https://en.wikipedia.org/wiki/Partition_problem
	int ret = 0;
	// Calculate sum of the elements in array
	int sum = std::accumulate(arr.begin(), arr.end(), 0);

	int n = arr.size();
	vector<bool> prev(sum + 1, false), curr(sum + 1);

	// Mark prev[0] = true as it is true
	// to make sum = 0 using 0 elements
	prev[0] = true;

	// Fill the subset table in
	// bottom up manner
	std::cout << "\n\t\t\t";
	for (int j = 0; j <= sum; j++) {
		std::cout << j << "\t";
	}
	std::cout << "\n";
	for (int i = 1; i <= n; i++) {
		for (int j = 0; j <= sum; j++) {
			if (j < arr[i - 1]) {
				curr[j] = prev[j];
			} else {
				// i = 3; a[3-1] = 5;
				// J = 10; prev[10 - a[3-1]];
				curr[j] = (prev[j] || prev[j - arr[i - 1]]);
			}
#if 0
			std::cout << "i = " << i << ", a[" << i - 1
				  << "] = " << arr[i - 1] << "\t\t";
			for (auto v : curr) {
				std::cout << (v ? "X" : "-") << "\t";
			}
			std::cout << "\n";
#endif
		}
#if 1

		std::cout << "i = " << i << ", a[" << i - 1
			  << "] = " << arr[i - 1] << "\t\t";
		for (auto v : curr) {
			std::cout << (v ? "X" : "-") << "\t";
		}
		std::cout << "\n";
#endif
		prev = curr;
	}
	result = curr[kVal];
	return ret;
}

inline int
sp_PartitionProblem_test()
{
	bool result = false;
	int kVal = 7;
	std::vector<int> output_track;
	// vector<int> arr = {1, 2,  2, 5};
	vector<int> vec = {2, 3, 3, 2, 1, 3};
	sp_PartitionProblem(vec, result, kVal, output_track);

	if (result) {
		cout << "True" << endl;
	} else {
		cout << "False" << endl;
	}
	return 0;
}
#endif // __sp_PartitionProblem__
