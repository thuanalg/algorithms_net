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

#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>   // C++98 (ISO/IEC 14882:1998)
using namespace std;


//template <typename T> using ProfitCoord = std::pair<int, int>;
//template <typename T>
inline int
sp_PartitionProblem( std::vector<int> &arr, bool &result, int kVal)
{
	int ret = 0;
	// Calculate sum of the elements in array
	int sum = std::accumulate(arr.begin(), arr.end(), 0);

	// If sum is odd, there cannot be two
	// subsets with equal sum
	if (sum % 2 != 0) {
		result = false;
		return ret;
	}

	//sum = sum / 2;
	//sum += 10;

	int n = arr.size();
	vector<bool> prev(sum + 1, false), curr(sum + 1);

	// Mark prev[0] = true as it is true
	// to make sum = 0 using 0 elements
	prev[0] = true;

	// Fill the subset table in
	// bottom up manner

	for (int i = 1; i <= n; i++) {
		for (int j = 0; j <= sum; j++) {
			if (j < arr[i - 1]) {
				curr[j] = prev[j];
			} else {
				int k = j - arr[i - 1];
				curr[j] = (prev[j] || prev[j - arr[i - 1]]);
				bool tmp = curr[j];
				int a = 0;
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
	int kVal = 4;
	// vector<int> arr = {1, 2,  2, 5};
	vector<int> vec = {5, 2, 1};
	sp_PartitionProblem(vec, result, kVal);

	if (result) {
		cout << "True" << endl;
	} else {
		cout << "False" << endl;
	}
	return 0;
}
#endif // __sp_PartitionProblem__
