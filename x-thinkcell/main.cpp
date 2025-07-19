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
	int rows = grid.size();
	int cols = grid[0].size();

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
	int rows = grid.size();
	int cols = grid[0].size();
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

/*

int
main()
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
*/

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
	int n = grid.size();
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
			if (grid[u][v] != INF &&
			    dist[v] > dist[u] + grid[u][v]) {
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

int
main()
{
	Grid grid = {{0, 1, 4, INF}, {INF, 0, 1, INF}, {INF, INF, 0, 1},
	    {INF, INF, INF, 0}};

	vector<int> parent(grid.size(), -1);
	vector<int> dist = dijkstra(grid, 0, parent);

	for (int i = 0; i < dist.size(); ++i) {
		cout << "Distance to " << i << ": ";
		if (dist[i] == INF) {
			cout << "-1 (unreachable)\n";
		} else {
			cout << dist[i] << ", Path: ";
			print_path(i, parent);
			cout << "\n";
		}
	}
}
