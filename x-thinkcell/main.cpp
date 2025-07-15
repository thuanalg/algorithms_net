#include <map>
#include <iostream>
#include <cassert>
#include <cassert>
#include <iostream>
void IntervalMapTest();
template<typename K, typename V>
class interval_map {
	friend void IntervalMapTest();
	V m_valBegin;
	std::map<K,V> m_map;
public:
	// constructor associates whole range of K with val
	template<typename V_forward>
	interval_map(V_forward&& val)
	: m_valBegin(std::forward<V_forward>(val))
	{}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	template<typename V_forward>
	void assign( K const& keyBegin, K const& keyEnd, V_forward&& val )
		requires (std::is_same<std::remove_cvref_t<V_forward>, V>::value)
	{
		// If the interval is empty, do nothing
		if (!(keyBegin < keyEnd)) {
			return;
		}

		// First, handle the case where keyBegin is not already in the map
		auto itLow = m_map.lower_bound(keyBegin);
		if (itLow == m_map.begin() || (--itLow)->second != val) {
			// Insert the boundary with the value val
			m_map[keyBegin] = std::forward<V_forward>(val);
		} else {
			// If keyBegin is already associated with the same value, no need to insert it again
			++itLow;
		}

		// Handle the case where keyEnd is not in the map
		auto itHigh = m_map.lower_bound(keyEnd);
		if (itHigh == m_map.end() || itHigh->first != keyEnd) {
			// Insert keyEnd with the value to stop the interval
			m_map[keyEnd] = itHigh == m_map.begin() ? m_valBegin : (--itHigh)->second;
		}

		// Remove any redundant elements between keyBegin and keyEnd
		auto it = m_map.lower_bound(keyBegin);
		++it; // Skip the first element
		while (it != m_map.lower_bound(keyEnd)) {
			it = m_map.erase(it);
		}
	}

	// look-up of the value associated with key
	V const& operator[]( K const& key ) const {
		auto it=m_map.upper_bound(key);
		if(it==m_map.begin()) {
			return m_valBegin;
		} else {
			return (--it)->second;
		}
	}
};
#include <stdio.h>
// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.
void IntervalMapTest() {
	interval_map<int, char> im('a');
	char val = 0;
	// Assign intervals
	im.assign(3, 5, 'b');
	im.assign(7, 10, 'c');
	im.assign(1, 4, 'd');

	val = im[3];
	fprintf(stdout, "val=%c.\n", val);
	assert(val == 'b');
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
int main(int argc, char *argv[]) {
	IntervalMapTest();
	return 0;
}
