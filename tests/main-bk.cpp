#include <map>
#include <iostream>
#include <cassert>
#include <cassert>
#include <iostream>
#include <utility>
#include <concepts>
#include <vector>
#include "sp_numberic.hpp"
#include "sp_greedy.hpp"
#include "sp_dijkstra.hpp"
#include "sp_SubarraySumEqualsK.hpp"
#include "sp_KadaneMaximumSubarraySum.hpp"
#include "sp_SubarraySumEqualsK.hpp"
#include "sp_LongestIncreasingSubsequence.hpp"
#include "sp_MaximumProfitStockTrading.hpp"
#include "sp_PartitionProblem.hpp"
#include "sp_IntervalMap.hpp"

	/*	
	// Verify the results
	assert(val == 'b');

	assert(im[5] == 'a'); // Default value
	assert(im[6] == 'a'); // Default value
	assert(im[8] == 'c');
	assert(im[10] == 'a'); // Default value

	std::cout << "Test passed!" << std::endl;
*/

// std::move,
// std::forward, 
// emplace_back
//std::ranges, 
// std::views
//std::concepts, 
// std::coroutines, 
//std::vector
//std::unordered_map

//int main(int argc, char *argv[]) {
//	IntervalMapTest();
//	return 0;
//}

#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
/*
int
main()
{
	std::vector<int> data = {1, 4, 2, 6, 3, 5};

	auto view =
	    data |
	    std::views::filter([](int x) { return x % 2 == 0; }) // lọc chẵn
	    | std::views::transform([](int x) { return x * 10; }); // nhân 10

	std::vector<int> result(view.begin(), view.end());

	std::ranges::sort(result); // sắp xếp

	for (int x : result) {
		std::cout << x << " ";
	}
}
*/
    //make_unique C++20
/*
| Tính năng / Thư viện | Thuộc C++ Phiên bản nào? | Ghi chú                                                                                                   |
| -------------------- | ------------------------ | --------------------------------------------------------------------------------------------------------- |
| `std::move`          | C++11                    | Dùng để chuyển quyền sở hữu tài nguyên (move semantics).                                                  |
| `std::forward`       | C++11                    | Dùng trong perfect forwarding để giữ nguyên giá trị l-value/r-value.                                      |
| `emplace_back`       | C++11                    | Thêm phần tử vào cuối container (ví dụ `std::vector`) bằng cách khởi tạo trực tiếp tại chỗ.               |
| `std::ranges`        | C++20                    | Khái niệm mới để làm việc với dãy giá trị (ranges) và kết hợp với `views`.                                |
| `std::views`         | C++20                    | Một phần của ranges; cung cấp các thao tác lazy (trì hoãn) như `filter`, `transform`, `take`, v.v.        |
| `std::concepts`      | C++20                    | Cho phép ràng buộc kiểu dữ liệu trong template bằng các khái niệm rõ ràng.                                |
| `std::require`       | C++20                    | .                                |
| `std::coroutines`    | C++20                    | - Cung cấp khả năng lập trình bất đồng bộ (asynchronous) hiệu quả, như `co_await`, `co_yield`, `co_return`. |
| `std::vector`        | C++98                    | Container động, quản lý mảng động tự động (một phần của STL).                                             |
| `std::unordered_map` | C++11                    | Bản ánh xạ (hash map), cho phép tra cứu O(1) trung bình.                                                  |
| `std::map`		   | C++11                    | Key có thứ tự                                                  |


| Tên               | Mô tả ngắn gọn                                | Phiên bản C++ |
| ----------------- | --------------------------------------------- | ------------- |
| `std::unique_ptr` | Chỉ có **1 owner** duy nhất. Không copy được. | C++11         |
| `std::shared_ptr` | Cho phép nhiều owner (ref-counted).           | C++11         |
| `std::weak_ptr`   | Tránh **vòng tham chiếu** với `shared_ptr`.   | C++11         |


| **Mục**                         | **Chi tiết**                                                                                         |
| ------------------------------- | ---------------------------------------------------------------------------------------------------- |
| **Giới thiệu**                  | Lambda là biểu thức khai báo hàm ẩn danh (anonymous function), có thể định nghĩa nội tuyến (inline). |
| **Hỗ trợ từ phiên bản**         | C++11 trở đi (mở rộng thêm ở C++14, C++17 và C++20).                                                 |
| **Cấu trúc tổng quát**          | `[capture](params) specifiers -> return_type { body }`                                               |
| **Thành phần chính**            |                                                                                                      |
| - `[capture]`                   | Cách bắt biến bên ngoài (by value `[=]`, by reference `[&]`, hoặc liệt kê `[x, &y]`)                 |
| - `(params)`                    | Danh sách tham số, giống hàm thông thường.                                                           |
| - `-> return_type` *(tuỳ chọn)* | Kiểu trả về (C++11 cần rõ, C++14 có thể suy diễn `auto`)                                             |
| - `{ body }`                    | Phần thân hàm                                                                                        |
| **Ví dụ đơn giản**              | `auto add = [](int a, int b) { return a + b; };`                                                     |
| **Gọi lambda**                  | `int result = add(2, 3);`  → `result = 5`                                                            |
| **Lambda không có capture**     | `[](){ std::cout << "Hello!\n"; }();`                                                                |
| **Lambda với capture by value** | `int x = 10; auto f = [=]() { return x + 5; };`                                                      |
| **Lambda với capture by ref**   | `int x = 10; auto f = [&]() { x += 5; }; f();`                                                       |
| **Lambda trong `std::sort`**    | `std::sort(v.begin(), v.end(), [](int a, int b) { return a < b; });`                                 |
| **Lambda trả về auto** (C++14)  | `auto square = [](auto x) { return x * x; };`                                                        |
| **Mutable lambda**              | Cho phép thay đổi biến bắt theo giá trị. `auto f = [=]() mutable { x++; };`                          |
| **Generic lambda (C++14+)**     | `auto f = [](auto a, auto b) { return a + b; };`                                                     |
| **Constexpr lambda (C++17)**    | `constexpr auto add = [](int a, int b) { return a + b; };`                                           |
| **Capturing `*this` (C++20)**   | `[*this] { return this->x; };`                                                                       |



| STT | Lớp `std::`               | Chức năng chính                          | Có từ phiên bản |
| --- | ------------------------- | ---------------------------------------- | --------------- |
| 1   | `std::vector`             | Mảng động                                | C++98           |
| 2   | `std::map`                | Cấu trúc ánh xạ key → value, sắp xếp     | C++98           |
| 3   | `std::unordered_map`      | Ánh xạ key → value, không sắp xếp (hash) | C++11           |
| 4   | `std::set`                | Tập hợp các phần tử duy nhất, có sắp xếp | C++98           |
| 5   | `std::unordered_set`      | Tập hợp duy nhất, không sắp xếp          | C++11           |
| 6   | `std::list`               | Danh sách liên kết đôi                   | C++98           |
| 7   | `std::forward_list`       | Danh sách liên kết đơn                   | C++11           |
| 8   | `std::array`              | Mảng có kích thước cố định               | C++11           |
| 9   | `std::deque`              | Hàng đợi hai đầu                         | C++98           |
| 10  | `std::string`             | Chuỗi ký tự                              | C++98           |
| 11  | `std::wstring`            | Chuỗi Unicode (wide characters)          | C++98           |
| 12  | `std::string_view`        | View không sở hữu dữ liệu chuỗi          | C++17           |
| 13  | `std::optional`           | Đại diện giá trị có thể có hoặc không    | C++17           |
| 14  | `std::variant`            | Union an toàn kiểu                       | C++17           |
| 15  | `std::any`                | Chứa bất kỳ kiểu nào                     | C++17           |
| 16  | `std::tuple`              | Bộ giá trị khác kiểu                     | C++11           |
| 17  | `std::pair`               | Cặp giá trị                              | C++98           |
| 18  | `std::bitset`             | Tập bit                                  | C++98           |
| 19  | `std::span`               | View đa chiều cho mảng, hỗ trợ range     | C++20           |
| 20  | `std::shared_ptr`         | Con trỏ thông minh chia sẻ               | C++11           |
| 21  | `std::unique_ptr`         | Con trỏ thông minh độc quyền             | C++11           |
| 22  | `std::weak_ptr`           | Con trỏ yếu (không giữ ownership)        | C++11           |
| 23  | `std::mutex`              | Mutex cho lập trình đa luồng             | C++11           |
| 24  | `std::shared_mutex`       | Mutex chia sẻ (đọc song song)            | C++17           |
| 25  | `std::jthread`            | Thread tự dọn dẹp (joining thread)       | C++20           |
| 26  | `std::thread`             | Đối tượng luồng                          | C++11           |
| 27  | `std::condition_variable` | Biến điều kiện cho đồng bộ hóa           | C++11           |
| 28  | `std::future`             | Cho kết quả không đồng bộ                | C++11           |
| 29  | `std::promise`            | Tạo dữ liệu cho `std::future`            | C++11           |
| 30  | `std::chrono::duration`   | Thời lượng/thời gian                     | C++11           |
| 31  | `std::remove_cvref_t`     | Bỏ cả const / volatile và reference khỏi T                     | C++20           |

*/

// 0. Main problem: Identify islands in a grid
// 1. List all islands
// 2. Find the largest island (by area = number of land cells)

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

using Grid = vector<vector<int>>;
using Coord = pair<int, int>;

//const vector<Coord> directions = {
//    {-1, 0}, {1, 0}, {0, -1}, {0, 1} // Up, Down, Left, Right
//};

const vector<Coord> directions = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Up, Down, Left, Right
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1} // Diagonals: NW, NE, SW, SE
};

// Depth-First Search to collect all connected land cells
void
dfs(int x, int y, const Grid &grid, vector<vector<bool>> &visited,
    vector<Coord> &island)
{
	size_t rows = grid.size();
	size_t cols = grid[0].size();

	if (x < 0 || y < 0 || x >= rows || y >= cols)
		return;
	if (grid[x][y] == 0 || visited[x][y])
		return;

	visited[x][y] = true;
	island.emplace_back(x, y);

	for (auto [dx, dy] : directions) {
		dfs(x + dx, y + dy, grid, visited, island);
	}
}





#include <iostream>
#include <queue>
#include <vector>
void
mycount();
using namespace std;
using myqueue = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, greater<>>;
int
main12()
{
	// priority_queue với min-heap (ưu tiên phần tử nhỏ nhất)
	myqueue pq;
	//priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
	// priority_queue<pair<int, int>, vector<pair<int, int>>, less<>> pq;

	// Thêm các phần tử (khoảng cách, đỉnh)
	pq.emplace(10, 2);
	pq.emplace(5, 0);
	pq.emplace(7, 1);
	pq.emplace(3, 3);

	// In ra các phần tử theo thứ tự ưu tiên
	while (!pq.empty()) {
		auto [dist, node] = pq.top();
		pq.pop();
		cout << "Node: " << node << ", Distance: " << dist << '\n';
	}
	//mycount();
	return 0;
	// rankdir=LR;
}

void
mycount()
{
//	std::priority_queue<std::pair<int, int>,
//	    std::vector<std::pair<int, int>>, greater<>> pq;
	myqueue pq;
	//using myqueue = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, greater<>>;
	std::unique_ptr<myqueue> ptr = std::make_unique<myqueue>();
	//std::unique_ptr<myqueue *> ptr1 = std::make_unique<myqueue *>(10);
	ptr->emplace(10, 2);
	ptr->emplace(5, 0);
	ptr->emplace(7, 1);
	ptr->emplace(3, 3);
	while (!ptr->empty()) {
		auto [dist, node] = ptr->top();
		ptr->pop();
		cout << "---Node: " << node << ", Distance: " << dist << '\n';
	}

}









template<typename T>
int
KnapsackProblem(std::vector<T> &vec)
	requires std::totally_ordered<T>
{
	//https://en.wikipedia.org/wiki/Knapsack_problem
	int ret = 0;
	return ret;
}



#pragma warning(disable : 4146)
#include <gmp.h>
int
mainpopo()
{
	printf("GMP version: %s\n", gmp_version);
	mpz_t a, b, result;
	mpz_init(a);
	mpz_init(b);
	mpz_init(result);
#define EXPOMENT			1000000000
	mpz_ui_pow_ui(result, 2, EXPOMENT);
	FILE *fp = fopen("D:/x/result_02.txt", "w+");
	char *data = (char *)malloc(EXPOMENT);
	memset(data, 0, EXPOMENT);
	if (fp) {
		//fwrite(data, ttt, 1, fp);
		size_t ttt =
		    gmp_snprintf(data, EXPOMENT, "2^1000000000:\n%Zd\n", result);
		int n = fwrite(data, 1, ttt, fp);
		fclose(fp);
	}
	mpz_clear(result);
	mpz_clear(b);
	mpz_clear(a);
	free(data);
	return 0;
}
/*
* MSYS2 -->> gendef .dll --> .def
* lib /def:libgmp-10.def /machine:x64 /out:libgmp.lib
* MSVC command -->> lib .def -->> .lib
* lib /def:libgmp-10.def /machine:x64 /out:libgmp.lib
* 
*/

// C++ program to partition a Set
// into Two Subsets of Equal Sum
// using space optimised
//#include <bits/stdc++.h>
#include <numeric> // cần khai báo thư viện này
#include <vector>
#include <iostream>

using namespace std;

// Returns true if arr[] can be partitioned in two
// subsets of equal sum, otherwise false
bool
equalPartition(vector<int> &arr)
{
	// Calculate sum of the elements in array
	int sum = accumulate(arr.begin(), arr.end(), 0);

	// If sum is odd, there cannot be two
	// subsets with equal sum
	if (sum % 2 != 0)
		return false;

	sum = sum / 2;
	sum += 10;

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
			}				
			else {
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
		std::cout << "i = " << i << ", a[" << i-1 << "] = " << arr[i-1]
			  << "\t\t";
		for (auto v : curr) {
			std::cout << (v ? "X" : "-") << "\t";
		}
		std::cout << "\n";
#endif
		prev = curr;

	}
	return prev[sum];
}

int
main___()
{
	//vector<int> arr = {1, 2,  2, 5};
	vector<int> arr = {5, 2, 1};
	if (equalPartition(arr)) {
		cout << "True" << endl;
	} else {
		cout << "False" << endl;
	}
	return 0;
}

int
main()
{
	
	//sp_greedy_test();
	//sp_dijkstra_test<double>();
	//sp_SubarraySumEqualsK_test();
	//sp_KadaneMaximumSubarraySum_test();
	//sp_LongestIncreasingSubsequence_test();
	//sp_MaximumProfitStockTrading_test();
	//sp_PartitionProblem_test();
	//sp_IntervalMap_test();
	sp_numberic_test();
	return 0;
}