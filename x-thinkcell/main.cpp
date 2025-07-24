#include <map>
#include <iostream>
#include <cassert>
#include <cassert>
#include <iostream>
#include <utility>
void IntervalMapTest();
template <typename K, typename V> class interval_map
{
	friend void IntervalMapTest();
	V m_valBegin;
	std::map<K, V> m_map;

      public:
	// constructor associates whole range of K with val
	template <typename V_forward>
	interval_map(V_forward &&val) : m_valBegin(std::forward<V_forward>(val))
	{
		int a = 0;
	}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	template <typename V_forward>
	void assign(K const &keyBegin, K const &keyEnd, V_forward &&val)
		requires(std::is_same<std::remove_cvref_t<V_forward>, V>::value)
	{
		// If the interval is empty, do nothing
		if (!(keyBegin < keyEnd)) {
			return;
		}

		// First, handle the case where keyBegin is not already in the
		// map
		auto itLow = m_map.lower_bound(keyBegin);
		// m_map.upper_bound
		if (itLow == m_map.begin() || (--itLow)->second != val) {
			// Insert the boundary with the value val
			// Have no any key
			m_map[keyBegin] = std::forward<V_forward>(val);
		} else {
			// If keyBegin is already associated with the same
			// value, no need to insert it again
			++itLow;
		}

		// Handle the case where keyEnd is not in the map
		auto itHigh = m_map.lower_bound(keyEnd);
		if (itHigh == m_map.end() ) {
			// Insert keyEnd with the value to stop the interval
			m_map[keyEnd] = (itHigh == m_map.begin())
					    ? m_valBegin
					    : (--itHigh)->second;
		} else if (itHigh->first != keyEnd) {
			auto tmp = itHigh->first;
			// Insert keyEnd with the value to stop the interval
			m_map[keyEnd] = (itHigh == m_map.begin())
					    ? m_valBegin
					    : (--itHigh)->second;
		}

		// Remove any redundant elements between keyBegin and keyEnd
		auto it = m_map.lower_bound(keyBegin);
		++it; // Skip the first element
		while (it != m_map.lower_bound(keyEnd)) {
			it = m_map.erase(it);
		}
	}

	// look-up of the value associated with key
	V const &operator[](K const &key) const
	{
		auto it = m_map.upper_bound(key);
		if (it == m_map.begin()) {
			return m_valBegin;
		} else {
			return (--it)->second;
		}
	}
	V const &find(K const &key) const
	{
		for (auto it : m_map) {
			std::cout << it.first << " ";
		}
		std::cout << std::endl;
		auto it = m_map.find(key);
		if (it == m_map.end()) {
			return m_valBegin;
		} else {
			return it->second;
		}

	};
};
#include <stdio.h>
// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.
void IntervalMapTest() {
	char myval = 'A';
	interval_map<int, char> im(myval);
	char val = 0;
	// Assign intervals
	val = im[4];
	myval = 'B';
	im.assign(3, 5, myval);
	val = im[4];
	im.assign(7, 10, 'c');
	val = im[8];
	im.assign(1, 4, 'd');
	im.assign(10, 20, 'D');

	val = im[3];
	fprintf(stdout, "val=%c.\n", val);
	assert(val == 'd');
	for(int j = 0; j < 20; j++) {
		val = im[j];
		printf("\nval[%d]=%c\n", j, val);
	}
	auto rt = im.find(2);
	rt = im.find(5);


	/*	
	// Verify the results
	assert(val == 'b');

	assert(im[5] == 'a'); // Default value
	assert(im[6] == 'a'); // Default value
	assert(im[8] == 'c');
	assert(im[10] == 'a'); // Default value

	std::cout << "Test passed!" << std::endl;
*/
}
class AAAA
{
	public:
	AAAA();
	virtual ~AAAA();

      private:
	int a, b;
};
/*
int main(int argc, char *argv[]) {
	IntervalMapTest();
	return 0;
}
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

// Find all islands in the grid
vector<vector<Coord>>
listAllIslands(const Grid &grid)
{
	size_t rows = grid.size();
	size_t cols = grid[0].size();
	vector<vector<bool>> visited(rows, vector<bool>(cols, false));
	vector<vector<Coord>> islands;

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			if (grid[i][j] == 1 && !visited[i][j]) {
				vector<Coord> island;
				dfs(i, j, grid, visited, island);
				//islands.push_back(move(island));
				islands.emplace_back(move(island));
			}

	return islands;
}



int
main2()
{
	Grid grid = {
	    {1, 1, 0, 0, 0},
	    {1, 0, 0, 1, 1},
	    {0, 0, 1, 1, 0},
	    {0, 0, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	    {0, 1, 0, 0, 0},
	};

	// 1. List all islands
	auto islands = listAllIslands(grid);
	cout << "Found " << islands.size() << " islands:\n";
	for (int i = 0; i < islands.size(); ++i) {
		cout << "- Island " << i + 1 << " with " << islands[i].size()
		     << " cells: ";
		for (auto [x, y] : islands[i])
			cout << "(" << x << "," << y << ") ";
		cout << '\n';
	}

	// 2. Find the largest island
	auto maxIslandIt = max_element(islands.begin(), islands.end(),
	    [](const auto &a, const auto &b) { return a.size() < b.size(); });

	if (maxIslandIt != islands.end()) {
		cout << "The largest island has " << maxIslandIt->size()
		     << " cells: ";
		for (auto [x, y] : *maxIslandIt)
			cout << "(" << x << "," << y << ") ";
		cout << '\n';
	}

	return 0;
}


#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <stack>

using namespace std;

using Grid = vector<vector<int>>;
const int INF = numeric_limits<int>::max();

vector<int>
dijkstra(const Grid &grid, int s, vector<int> &parent)
{
	size_t n = grid.size();
	vector<int> dist(n, INF);
	vector<bool> visited(n, false);
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

	dist[s] = 0;
	parent[s] = -1; // root has no parent
	pq.emplace(0, s);

	while (!pq.empty()) {
		auto [d, u] = pq.top();
		pq.pop();
		if (visited[u])
			continue;
		visited[u] = true;

		for (int v = 0; v < n; ++v) {
			if (grid[u][v] == INF) {
				continue;
			}
			if ( dist[v] > dist[u] + grid[u][v]) {
				dist[v] = dist[u] + grid[u][v];
				parent[v] = u; // save parent
				pq.emplace(dist[v], v);
			}
		}
	}

	return dist;
}

// In đường đi từ s đến v
void
print_path(int v, const vector<int> &parent)
{
	if (parent[v] == -1) {
		cout << v;
		return;
	}
	print_path(parent[v], parent);
	cout << " -> " << v;
}
//http://www.webgraphviz.com/
int
main1()
{
//	Grid grid = {
//		{0, 1, 4, INF, INF, 17}, //0
//		{INF, 0, 10, INF, 1, INF}, //1
//		{INF, INF, 0, 1, 1, 12},//2
//	    {INF, INF, INF, 0, INF, 2},//3
//	    {INF, INF, INF, 10, 0, 5},//4
//	    {INF, INF, INF, INF, INF, 0}, // 5
//	};

//	Grid grid = {
//	    {0, 1, 4, INF, INF, 17}, // 0
//	    {INF, 0, 10, INF, 1, INF}, // 1
//	    {INF, INF, 0, 1, 1, 12}, // 2
//	    {INF, INF, INF, 0, INF, 2}, // 3
//	    {INF, INF, INF, 10, 0, 6}, // 4
//	    {INF, INF, INF, INF, INF, 0}, // 5
//	};

	Grid grid = {
	    {0, 1, 4, INF, INF, 1}, // 0
	    {INF, 0, 1, INF, 1, INF}, // 1
	    {INF, INF, 0, 1, 1, 12}, // 2
	    {INF, INF, INF, 0, INF, 2}, // 3
	    {INF, INF, INF, 10, 0, 6}, // 4
	    {INF, INF, INF, INF, INF, 0}, // 5
	};


//	Grid grid = {
//    {0, 1, 4, INF, INF, 1}, // 0
//	    {INF, 0, 10, INF, 1, INF}, // 1
//	    {INF, INF, 0, 1, 1, 12}, // 2
//	    {INF, INF, INF, 0, INF, 2}, // 3
//	    {INF, INF, INF, 10, 0, 5}, // 4
//	    {INF, INF, INF, INF, INF, 0}, // 5
//	};
	vector<int> parent(grid.size(), -1);
	vector<int> dist = dijkstra(grid, 0, parent);
	size_t dest = dist.size() - 1;
	//	if (dist[dest] < INF) {
//		cout << "Distance to " << dest << ": ";
//		cout << dist[dest] << ", Path: ";
//		print_path(dest, parent);
//		cout << "\n";	
//	}
	for (int i = 0; i < dist.size(); ++i) {
		//if (i < dist.size() - 1) {
		//	continue;
		//}
		cout << "Distance to " << i << ": ";
		if (dist[i] == INF) {
			cout << "-1 (unreachable)\n";
		} else {
			cout << dist[i] << ", Path: ";
			print_path(i, parent);
			cout << "\n";
		}
	}
	return 0;
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
	requires std::is_arithmetic_v<T>
{
	int ret = 0;

	if (vec.size() < 1) {
		return ret;
	}
	// C++20
	std::ranges::sort(vec, [](const auto &a, const auto &b) { 
			return a.size() < b.size(); 
		});
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
	if (vec.size() < 1) {
		return ret;
	}
	for (size_t index = 0; index < vec.size() - 1; ++index) {
		if (vec[index].size() == vec[index + 1].size()) {
			size_t i = vec[index].size() - 1;
			size_t j = vec[index + 1].size() - 1;
			size_t fd = index;
			if (vec[index][i] < vec[index + 1][j]) {
				fd = index + 1;
			} 
			vec.erase(vec.begin() + fd);
		}
	}
	return ret;
}

template <typename T>
int
longest_increasing_subsequence(const std::vector<T> &nums)
{
	int ret = 0;
	std::vector<T> lis; // 
	std::vector<std::vector<T>> arrlis; // 
	std::vector<std::vector<T>> arrlistmp; // 
	if (nums.size() == 0) {
		return ret;
	}
	arrlis.push_back(lis);

	for (auto &num : nums) {
		arrlistmp.clear();
		for (auto &tmp : arrlis) {
			auto it = std::lower_bound(tmp.begin(), tmp.end(), num);
			if (it == tmp.end()) {
				tmp.push_back(num); // Expand list
			} 
			else {
				auto testit = tmp.end();
				testit--;
				if (*testit == num && 0) {
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
		std::cout << v << "\t";
	}
	std::cout << std::endl;

	return (int) lis.size();
}
//The principle is to make the last element of the list become smaller and length become larger
//2       3       7       18      101
int
main()
{
#if 1
	std::vector<double> data = { 10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101, 101, 101, 101, 101, 
		15, 16, 17, 19, 20, 1, 2, 3, 4, 5, 6, 7, 7.1, 7.2, 8, 9, 200.0};
	//std::vector<float> data = {10, 9, 2, 5, 2.5, 7, 101, 18, 101, 101, 15, 16, 17, 19, 20, 1, 2, 3, 4, 5, };
#else
	std::vector<int> data = {10, 9, 2, 5, 3, 7, 101, 18, 101, 101, 15};
#endif
	std::cout << "Length of LIS: " << longest_increasing_subsequence(data)
		  << '\n';
	return 0;
}
