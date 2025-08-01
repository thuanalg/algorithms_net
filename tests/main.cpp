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
/*
## #1. *
	*Dãy con liên tiếp có tổng lớn
	nhất(Maximum Subarray Sum) *
	*[*] -
    **Mô tả : **Tìm dãy con liên tiếp có tổng lớn nhất trong một mảng số cho
	      trước.Đây là bài toán cổ điển và được áp dụng trong phân tích lợi
	      nhuận từ các chỉ số tài chính,
    dự báo giá trị tài sản.-
	**Ứng dụng : **Tính toán lợi nhuận tối đa trong các chiến lược đầu tư.-
	**Thuật toán : **Kadane’s Algorithm(O(n)).
*/

template <typename T> 
using myIndexPair = std::pair<int, int>;

template <typename K, typename V>
using mydataaa = std::map<K, V, std::greater<>>;

template <typename T>

int
Kadane_Algorithm( std::vector<T> &data, mydataaa<T, myIndexPair<int>> &output) 
	requires std::is_arithmetic_v<T>
{
	//https://en.wikipedia.org/wiki/Maximum_subarray_problem
	// https://en.wikipedia.org/wiki/Maximum_subarray_problem#Kadane's_algorithm
	//C++20
	int ret = 0;
	int s = 0;
	int i = 0;
	output.clear();
	//int arr[] = {-2, -3, 4, -1, -2, 10, 5, -3};
	do {
		if (data.size() < 1) {
			ret = -1;
			break;
		}
		auto MyaddLamda = []<typename U>(U &a, U &b)
			requires(std::is_same<std::remove_cvref_t<T>, U>::value)
		{ 
			a += b; 
		};
		T max_so_far = 0;
		T max_ending_here = 0;
		for (auto v : data) {
			if (max_ending_here < 0) {
				max_ending_here = v;
				s = i;
			} else {
				//max_ending_here += v;
				MyaddLamda(max_ending_here, v);

			}
			if (max_ending_here > max_so_far) {
				max_so_far = max_ending_here;
				//Memorize a pair
				output[max_so_far] = std::make_pair(s, i);
			}
			++i;
		}
	} while (0);
	//int arr[] = {-2, -3, 4, -1, -2, 1, 5, -3};
	return ret;
}
int
testKadane_Algorithm()
{
	std::vector<double> dta = {-2, -3, 4, -1, -2, 10, 5.121, -3, 4, 0, -1, 2.0, -20, 100};
	//std::vector<double> dta = {100, -3, 4, -1, -2, 1, 5.121, -3};
	mydataaa<double, myIndexPair<int>> output;
	int ret = Kadane_Algorithm(dta, output);
	for (auto it : output) {
		std::cout << "val: " << it.first << ", \tindex: ("
			  << it.second.first << ", " << it.second.second
			  << ")." << std::endl;
	}

	return 0;
}

int
mainsa()
{
	return testKadane_Algorithm();

}


template <typename T>
int LongestIncreasingSubsequence
(std::vector<T> &data, std::vector<std::vector<T>> &output) 
requires std::is_arithmetic_v<T>
{
	int ret = 0, i = 0, j = 0;
	size_t n = data.size();



	for (auto v : data) {
		std::vector<T> tmp;
		tmp.push_back(v);
		for (j = i + 1; j < n; ++j) {
			if (tmp.size()) {
				auto it = tmp.end();
				--it;
				if (*it <= data[j]) {
					tmp.push_back(data[j]);
				}
			} 
		}
		++i;
	}
	return ret;
}

int
test_longest_sub()
{
	int ret = 0;
	std::vector<std::vector<double>> output;
	std::vector<double> data = { -3, 4, -1, -2, 1, 5.121, -3};
	ret = LongestIncreasingSubsequence(data, output);
	return ret;
}

int
main121()
{
	return test_longest_sub();
}


#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
int
clean_trash(std::vector<std::vector<T>> &vec)
	requires(std::is_arithmetic_v<T> || std::totally_ordered<T>)
{
	int ret = 0;

	if (vec.size() < 1) {
		return ret;
	}
	// C++20
	std::ranges::sort(vec, [](const auto &a, const auto &b) { 
			if (a.size() != b.size()) return a.size() < b.size(); 
			int i = 0, n = a.size();
			for (i = 0; i < n; ++i) {
				if (a[i] != b[i]) {
					return a[i] < b[i];
				}
			}
			return !!0; 
		});


	//for (size_t index = 0; index < vec.size() - 1; ++index) {
	//	if (vec[index].size() == vec[index + 1].size()) {
	//		size_t i = vec[index].size() - 1;
	//		size_t j = vec[index + 1].size() - 1;
	//		size_t fd = index;
	//		if (vec[index][i] < vec[index + 1][j]) {
	//			fd = index + 1;
	//		} 
	//		vec.erase(vec.begin() + fd);
	//	}
	//}
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
longest_increasing_subsequence(const std::vector<T> &nums) 
	requires (std::is_arithmetic_v<T> ||  std::totally_ordered<T>)
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
		//if (num == 11) {
		//	//int a = num;
		//}
		arrlistmp.clear();
		for (auto &tmp : arrlis) {
			auto it = std::lower_bound(tmp.begin(), tmp.end(), num);
			if (it == tmp.end()) {
				tmp.push_back(num); // Expand list
			} 
			else {
				auto testit = tmp.end();
				testit--;
				if (*testit == num && 1) {
					tmp.push_back(num); // Expand list
				} 
				else {
					std::vector<T> copied(tmp.begin(), it);
					copied.push_back(num);
					arrlistmp.push_back(std::move(copied));
				}
			}
		}
		if (arrlistmp.size()) {
			clean_trash(arrlistmp);
			std::copy(arrlistmp.begin(), arrlistmp.end(),
			    std::back_inserter(arrlis));
		}
		clean_trash(arrlis);
		int sdsd = 0;
	}
	std::cout << std::endl;

	if (arrlis.size()) {
		lis.clear();
		auto it = arrlis.end();
		--it;
		lis = *it;
	}
	std::cout << std::endl;
	for (auto v : lis) {
		std::cout << v << "\t<-->\t";
	}
	std::cout << std::endl;

	return (int)lis.size();
}
//The principle is to make the last element of the list become smaller and length become larger
//2       3       7       18      101
int
mainasa()
{
#if 0
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101, 101, 101, 101, 101, 101,
		15, 16, 17, 19, 20, 1, -100, 2, 3, 4, 5, 6, 7, 7.1, 7.2, 8, 9.0,10,   };
	//std::vector<float> data = {10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, };
#else
	//std::vector<sp_numberic<int>> data;
	//std::vector<int> data = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
	
	std::vector<sp_numberic<double>> data = {{0.0, 0.2}, {8, 0}, {4, 0}, {12, 0},
	    {2, 0}, {10, 0}, {6, -1}, {6, -2.1}, {14, 0}, {1, -1.1}, {9, 0}, {5, 0},
	    {13, 0},
	    {3, 0}, {11, 0}, {7, 0}, {15, 0}};

	//std::vector<sp_numberic<int>> data = {{0, 1}, {0, 2}};
	//data.push_back(sp_numberic<int>(0, 0));
	//data.push_back(sp_numberic<int>(1, 1));
	sp_numberic<double> wqwq(1, 0);
	bool k = !!wqwq;

#endif
	std::cout << "Length of LIS: " << longest_increasing_subsequence(data) << '\n';
	return 0;
}



#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

template <typename T>
using mycoord = std::pair<int, int>;
template <typename T>

int
SubarraySumEqualsK(std::vector<T>& vec, T& k, std::vector <mycoord<int>>& output)
	requires std::is_arithmetic_v<T>
{
	std::unordered_map<T, int> umap;
	T sum = 0;
	int i = 0;
	for (auto& v : vec) {
		sum += v;
		if (sum == k) {
			output.emplace_back(0, i);
		}
		if (umap.find(sum - k) != umap.end()) {
			int j = i;
			T sumj = 0;
			for (; j >= 0; --j) {
				sumj += vec[j];
				if (sumj == k) {
					output.emplace_back(j, i);
					break;
				}
			}
		}
		umap[sum]++;
		++i;
	}
	return 0;
}

int
test_SubarraySumEqualsK()
{
#if 0
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101,
		101, 101, 101, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 7,
		7.1, 7.2, 8, 9.0 };
	// std::vector<float> data = {10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101,
	// 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, };
#else
	std::vector<int> data = { 7, 0, -7, 7, 3, 7, 101, 18, 101, 101, 15 };
#endif
	std::vector<mycoord<int>> output;
	int k = 7;
	int ret = SubarraySumEqualsK(data, k, output);
	return ret;
}

int
mainsdsds()
{
	return test_SubarraySumEqualsK();
}
// Constraints:
// 1. Buy first, sell late
// 2. Time is shortest
template <typename T>
int FindingMaximumProfitStockTrading( std::vector<T> &vec, T& output) requires std::is_arithmetic_v<T>
{
	T min_price = 100000, max_profit = -1;
	int i = 0;
	int start = -1;
	int end = -1;
	std::pair<int, int> prev(-1, -1), curr(-1, -1);
	for (auto v : vec) {
		if (v < min_price) {
			start = i;
			min_price = v;
			if (prev.first < 0) {
				prev.first = i;
				curr.first = i;
			} else if (curr.first == prev.first) {
				curr.first = i;
			} else if (curr.first > prev.first) {
				//if ()
				if (curr.first < curr.second) {
					prev.first = curr.first;
				}
				curr.first = i;
			}
		}
		else if (v - min_price > max_profit) {
			max_profit = (v - min_price);
			end = i;
			if (prev.second < 0) {
				prev.second = i;
				curr.second = i;
			}
			else if (prev.second == curr.second) {
				curr.second = i;
			}	
			else if (prev.second < curr.second) {
				prev.second = curr.second;
				curr.second = i;
			}	
		}
		++i;
	}
	output = max_profit;
	return 0;
}

int
test_FindingMaximumProfitStockTrading()
{
#if 0
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 101, 101, 18, 101,
		101, 101, 101, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 7,
		7.1, 7.2, 8, 9.0 };
	// std::vector<float> data = {10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101,
	// 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, };
#else
	std::vector<double> data = {
	    7, 0, 7, 7, 3, 7, 101, 18, 0, 0, 101, 0, 101, 101, 101, };
#endif
	std::vector<mycoord<int>> output;
	double k = 0;
	int ret = FindingMaximumProfitStockTrading(data, k);
	return ret;
}

int
maindsd54s()
{
	return test_FindingMaximumProfitStockTrading();
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


int
main1235()
{
	sp_numberic<double> a(1.0, 2.0);
	sp_numberic<double> b(-1.0, 2.0);
	sp_numberic<double> c(-1.0, -2.0);
	a += b;
	a -= c;
	a *= b;
	a /= c; 
	a + b;
	++a;
	std::cout << a;
	a++;
	std::cout << a;
	return 0;
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
	sp_IntervalMap_test();
	return 0;
}