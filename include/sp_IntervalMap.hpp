#ifndef __sp_IntervalMap__
#define __sp_IntervalMap__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <unordered_map>
#include <map>
#include <cassert>
#include <utility>
#include <concepts>

using namespace std;

int sp_IntervalMap_test();
template <typename K, typename V> class IntervalMap
{
	friend int sp_IntervalMap_test();
	V m_valBegin;
	std::map<K, V> m_map;

public:
	// constructor associates whole range of K with val
	template <typename V_forward>
	IntervalMap(V_forward &&val) : m_valBegin(std::forward<V_forward>(val))
	{
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

int sp_IntervalMap_test() {
	char myval = 'A';
	IntervalMap<int, char> im(myval);
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
	for (int j = 0; j < 20; j++) {
		val = im[j];
		printf("\nval[%d]=%c\n", j, val);
	}
	auto rt = im.find(2);
	rt = im.find(5);
	return 0;
}

#endif // __sp_IntervalMap__
